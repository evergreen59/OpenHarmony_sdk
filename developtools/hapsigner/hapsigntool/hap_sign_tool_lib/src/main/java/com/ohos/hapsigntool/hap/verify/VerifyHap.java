/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.ohos.hapsigntool.hap.verify;

import com.ohos.hapsigntool.api.model.Options;
import com.ohos.hapsigntool.hap.entity.Pair;
import com.ohos.hapsigntool.hap.entity.SigningBlock;
import com.ohos.hapsigntool.hap.exception.HapFormatException;
import com.ohos.hapsigntool.hap.exception.SignatureNotFoundException;
import com.ohos.hapsigntool.utils.FileUtils;
import com.ohos.hapsigntool.utils.HapUtils;
import com.ohos.hapsigntool.utils.ParamConstants;
import com.ohos.hapsigntool.utils.StringUtils;
import com.ohos.hapsigntool.zip.ByteBufferZipDataInput;
import com.ohos.hapsigntool.zip.RandomAccessFileZipDataInput;
import com.ohos.hapsigntool.zip.ZipDataInput;
import com.ohos.hapsigntool.zip.ZipFileInfo;
import com.ohos.hapsigntool.zip.ZipUtils;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.openssl.jcajce.JcaPEMWriter;
import org.bouncycastle.util.Arrays;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.Security;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Class of verify hap.
 *
 * @2021/12/23
 */
public class VerifyHap {
    private static final Logger LOGGER = LogManager.getLogger(VerifyHap.class);
    private static final int ZIP_HEAD_OF_SIGNING_BLOCK_LENGTH = 32;
    private static final int ZIP_HEAD_OF_SIGNING_BLOCK_COUNT_OFFSET_REVERSE = 28;
    private static final int ZIP_HEAD_OF_SUBSIGNING_BLOCK_LENGTH = 12;

    private final boolean printCert;

    public VerifyHap() {
        this(true);
    }

    public VerifyHap(boolean printCert) {
        this.printCert = printCert;
    }

    static {
        Security.addProvider(new BouncyCastleProvider());
    }

    /**
     * Check whether parameters are valid
     *
     * @param options input parameters used to verify hap.
     * @return true, if all parameters are valid.
     */
    public boolean checkParams(Options options) {
        if (!options.containsKey(ParamConstants.PARAM_VERIFY_CERTCHAIN_FILE)) {
            LOGGER.error("Missing parameter: {}", ParamConstants.PARAM_VERIFY_CERTCHAIN_FILE);
            return false;
        }
        if (!options.containsKey(ParamConstants.PARAM_VERIFY_PROFILE_FILE)) {
            LOGGER.error("Missing parameter: {}", ParamConstants.PARAM_VERIFY_PROFILE_FILE);
            return false;
        }
        if (!options.containsKey(ParamConstants.PARAM_VERIFY_PROOF_FILE)) {
            LOGGER.warn("Missing parameter: {}", ParamConstants.PARAM_VERIFY_PROOF_FILE);
        }
        return true;
    }

    /**
     * verify hap file.
     *
     * @param options input parameters used to verify hap.
     * @return true, if verify successfully.
     */
    public boolean verify(Options options) {
        VerifyResult verifyResult;
        try {
            if (!checkParams(options)) {
                LOGGER.error("Check params failed!");
                throw new IOException();
            }
            String filePath = options.getString(ParamConstants.PARAM_BASIC_INPUT_FILE);
            String outputCertPath = options.getString(ParamConstants.PARAM_VERIFY_CERTCHAIN_FILE);
            if (StringUtils.isEmpty(filePath)) {
                LOGGER.error("Not found verify file path!");
                throw new IOException();
            }
            File signedFile = new File(filePath);
            if (!checkSignFile(signedFile)) {
                LOGGER.error("Check input signature hap false!");
                throw new IOException();
            }
            verifyResult = verifyHap(filePath);
            if (!verifyResult.isVerified()) {
                LOGGER.error("verify: {}", verifyResult.getMessage());
                throw new IOException();
            }

            writeCertificate(outputCertPath, verifyResult.getCertificates());
        } catch (IOException e) {
            LOGGER.error("Write certificate chain error", e);
            return false;
        }

        String outputProfileFile = options.getString(ParamConstants.PARAM_VERIFY_PROFILE_FILE);
        String outputProofFile = options.getString(ParamConstants.PARAM_VERIFY_PROOF_FILE);
        String outputPropertyFile = options.getString(ParamConstants.PARAM_VERIFY_PROPERTY_FILE);
        try {
            outputOptionalBlocks(outputProfileFile, outputProofFile, outputPropertyFile, verifyResult);
        } catch (IOException e) {
            LOGGER.error("Output optional blocks error", e);
            return false;
        }

        LOGGER.info("verify: {}", verifyResult.getMessage());
        return true;
    }

    private void writeCertificate(String destFile, List<X509Certificate> certificates) throws IOException {
        try (JcaPEMWriter writer = new JcaPEMWriter(new FileWriter(destFile))) {
            for (final X509Certificate cert : certificates) {
                writer.write(cert.getSubjectDN().toString() + System.lineSeparator());
                writer.writeObject(cert);
            }
            LOGGER.info("Write certificate chain success!");
        }
    }

    private void outputOptionalBlocks(String outputProfileFile, String outputProofFile, String outputPropertyFile,
        VerifyResult verifyResult) throws IOException {
        List<SigningBlock> optionalBlocks = verifyResult.getOptionalBlocks();
        if (optionalBlocks != null && optionalBlocks.size() > 0) {
            for (SigningBlock optionalBlock : optionalBlocks) {
                int type = optionalBlock.getType();
                switch (type) {
                    case HapUtils.HAP_PROFILE_BLOCK_ID:
                        writeOptionalBytesToFile(optionalBlock.getValue(), outputProfileFile);
                        break;
                    case HapUtils.HAP_PROOF_OF_ROTATION_BLOCK_ID:
                        writeOptionalBytesToFile(optionalBlock.getValue(), outputProofFile);
                        break;
                    case HapUtils.HAP_PROPERTY_BLOCK_ID:
                        writeOptionalBytesToFile(optionalBlock.getValue(), outputPropertyFile);
                        break;
                    default:
                        throw new IOException("Unsupported Block Id: 0x" + Long.toHexString(type));
                }
            }
        }
    }

    private void writeOptionalBytesToFile(byte[] data, String outputFile) throws IOException {
        if (outputFile == null || outputFile.isEmpty()) {
            return;
        }
        try (OutputStream out = new FileOutputStream(outputFile)) {
            out.write(data);
            out.flush();
        }
    }

    private boolean checkSignFile(File signedFile) {
        try {
            FileUtils.isValidFile(signedFile);
        } catch (IOException e) {
            LOGGER.error("signedFile is invalid.", e);
            return false;
        }
        return true;
    }

    /**
     * Verify signature of hap.
     *
     * @param hapFilePath path of hap file
     * @param outCertPath path to output certificate file
     * @param outProvisionFile path to output provision file
     * @return verify result
     */
    public VerifyResult verifyHap(String hapFilePath, String outCertPath, String outProvisionFile) {
        VerifyResult verifyResult = verifyHap(hapFilePath);
        if (!verifyResult.isVerified()) {
            return verifyResult;
        }
        List<X509Certificate> certificates = verifyResult.getCertificates();
        try {
            writeCertificate(outCertPath, certificates);
            outputOptionalBlocks(outProvisionFile, null, null, verifyResult);
        } catch (IOException e) {
            LOGGER.error("Write certificate chain or profile error", e);
            verifyResult.setResult(false);
            return verifyResult;
        }
        return verifyResult;
    }

    /**
     * Verify hap file.
     *
     * @param hapFilePath path of hap file.
     * @return true, if verify successfully.
     */
    public VerifyResult verifyHap(String hapFilePath) {
        VerifyResult result;
        try (RandomAccessFile fle = new RandomAccessFile(hapFilePath, "r")) {
            ZipDataInput hapFile = new RandomAccessFileZipDataInput(fle);
            ZipFileInfo zipInfo = ZipUtils.findZipInfo(hapFile);
            long eocdOffset = zipInfo.getEocdOffset();
            if (ZipUtils.checkZip64EoCDLocatorIsPresent(hapFile, eocdOffset)) {
                String errorMsg = "ZIP64 format not supported!";
                LOGGER.error(errorMsg);
                return new VerifyResult(false, VerifyResult.RET_UNSUPPORTED_FORMAT_ERROR, errorMsg);
            }
            HapUtils.HapSignBlockInfo hapSigningBlockAndOffsetInFile = HapUtils.findHapSigningBlock(hapFile, zipInfo);
            ByteBuffer signingBlock = hapSigningBlockAndOffsetInFile.getContent();
            signingBlock.order(ByteOrder.LITTLE_ENDIAN);
            Pair<ByteBuffer, List<SigningBlock>> blockPair = getHapSignatureSchemeBlockAndOptionalBlocks(signingBlock);
            ByteBuffer signatureSchemeBlock = blockPair.getFirst();
            List<SigningBlock> optionalBlocks = blockPair.getSecond();
            Collections.reverse(optionalBlocks);
            long signingBlockOffset = hapSigningBlockAndOffsetInFile.getOffset();
            ZipDataInput beforeHapSigningBlock = hapFile.slice(0, signingBlockOffset);
            ZipDataInput centralDirectoryBlock = hapFile.slice(zipInfo.getCentralDirectoryOffset(),
                zipInfo.getCentralDirectorySize());
            ByteBuffer eocdBbyteBuffer = zipInfo.getEocd();
            ZipUtils.setCentralDirectoryOffset(eocdBbyteBuffer, signingBlockOffset);
            ZipDataInput eocdBlock = new ByteBufferZipDataInput(eocdBbyteBuffer);
            HapVerify verifyEngine = new HapVerify(beforeHapSigningBlock, signatureSchemeBlock,
                centralDirectoryBlock, eocdBlock, optionalBlocks);
            verifyEngine.setPrintCert(printCert);
            result = verifyEngine.verify();
            result.setSignBlockVersion(hapSigningBlockAndOffsetInFile.getVersion());
        } catch (IOException e) {
            LOGGER.error("Verify Hap has IO error!", e);
            result = new VerifyResult(false, VerifyResult.RET_IO_ERROR, e.getMessage());
        } catch (SignatureNotFoundException e) {
            LOGGER.error("Verify Hap failed, signature not found.", e);
            result = new VerifyResult(false, VerifyResult.RET_SIGNATURE_NOT_FOUND_ERROR, e.getMessage());
        } catch (HapFormatException e) {
            LOGGER.error("Verify Hap failed, unsupported format hap.", e);
            result = new VerifyResult(false, VerifyResult.RET_UNSUPPORTED_FORMAT_ERROR, e.getMessage());
        }
        return result;
    }

    private Pair<ByteBuffer, List<SigningBlock>> getHapSignatureSchemeBlockAndOptionalBlocks(ByteBuffer hapSigningBlock)
            throws SignatureNotFoundException {
        try {
            ByteBuffer header = HapUtils.reverseSliceBuffer(
                hapSigningBlock,
                hapSigningBlock.capacity() - ZIP_HEAD_OF_SIGNING_BLOCK_LENGTH,
                hapSigningBlock.capacity());
            ByteBuffer value = HapUtils.reverseSliceBuffer(hapSigningBlock, 0,
                hapSigningBlock.capacity() - ZIP_HEAD_OF_SIGNING_BLOCK_LENGTH);

            byte[] signatureValueBytes = new byte[value.capacity()];
            value.get(signatureValueBytes, 0, signatureValueBytes.length);
            signatureValueBytes = Arrays.reverse(signatureValueBytes);
            header.position(ZIP_HEAD_OF_SIGNING_BLOCK_COUNT_OFFSET_REVERSE); // position to the block count offset
            int blockCount = header.getInt();
            int current = value.capacity() - ZIP_HEAD_OF_SUBSIGNING_BLOCK_LENGTH * blockCount;
            value.position(current);

            int blockType = -1;
            int blockLength = -1;
            int blockOffset = -1;
            ByteBuffer hapSigningPkcs7Block = null;
            List<SigningBlock> optionalBlocks = new ArrayList<SigningBlock>();
            for (int i = 0; i < blockCount; i++) {
                blockOffset = value.getInt();
                blockLength = value.getInt();
                blockType = value.getInt();
                if (blockOffset + blockLength > signatureValueBytes.length) {
                    throw new SignatureNotFoundException("block end pos: " + (blockOffset + blockLength) +
                        " is larger than block len: " + signatureValueBytes.length);
                }
                if (HapUtils.getHapSignatureOptionalBlockIds().contains(blockType)) {
                    byte[] blockValue = Arrays.copyOfRange(signatureValueBytes, blockOffset, blockOffset + blockLength);
                    optionalBlocks.add(new SigningBlock(blockType, blockValue));
                }
                if (blockType == HapUtils.HAP_SIGNATURE_SCHEME_V1_BLOCK_ID) {
                    byte[] result = Arrays.copyOfRange(signatureValueBytes, blockOffset, blockOffset + blockLength);
                    hapSigningPkcs7Block = ByteBuffer.wrap(result);
                }
            }
            return Pair.create(hapSigningPkcs7Block, optionalBlocks);
        } finally {
            hapSigningBlock.clear();
        }
    }
}