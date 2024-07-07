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

package com.ohos.hapsigntool.hap.sign;

import com.ohos.hapsigntool.api.model.Options;
import com.ohos.hapsigntool.hap.config.SignerConfig;
import com.ohos.hapsigntool.hap.entity.Pair;
import com.ohos.hapsigntool.hap.entity.SigningBlock;
import com.ohos.hapsigntool.hap.exception.SignatureException;
import com.ohos.hapsigntool.utils.HapUtils;
import com.ohos.hapsigntool.zip.ZipDataInput;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.DigestException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.jar.JarOutputStream;

/**
 * Hap Signature Scheme signer
 *
 * @since 2021/12/21
 */
public abstract class SignHap {
    private static final int STORED_ENTRY_SO_ALIGNMENT = 4096;
    private static final int BUFFER_LENGTH = 4096;
    private static final int BLOCK_COUNT = 4;
    private static final int BLOCK_SIZE = 8;
    private static final int BLOCK_MAGIC = 16;
    private static final int BLOCK_VERSION = 4;
    private static final long INIT_OFFSET_LEN = 4L;
    private static final int OPTIONAL_TYPE_SIZE = 4;
    private static final int OPTIONAL_LENGTH_SIZE = 4;
    private static final int OPTIONAL_OFFSET_SIZE = 4;

    private SignHap() {}

    public static int getBlockSize() {
        return BLOCK_SIZE;
    }

    /**
     * Get all entries' name from hap which is opened as a jar-file.
     *
     * @param hap input hap-file which is opened as a jar-file.
     * @return list of entries' names.
     */
    public static List<String> getEntryNamesFromHap(JarFile hap) {
        List<String> result = new ArrayList<String>();
        for (Enumeration<JarEntry> e = hap.entries(); e.hasMoreElements();) {
            JarEntry entry = e.nextElement();
            if (!entry.isDirectory()) {
                result.add(entry.getName());
            }
        }
        return result;
    }

    /**
     * Copy the jar file and align the storage entries.
     *
     * @param entryNames list of entries' name
     * @param in input hap-file which is opened as a jar-file.
     * @param out output stream of jar.
     * @param timestamp ZIP file timestamps
     * @param defaultAlignment default value of alignment.
     * @throws IOException io error.
     */
    public static void copyFiles(List<String> entryNames, JarFile in,
        JarOutputStream out, long timestamp, int defaultAlignment) throws IOException {
        Collections.sort(entryNames);
        long offset = INIT_OFFSET_LEN;
        for (String name : entryNames) {
            JarEntry inEntry = in.getJarEntry(name);
            if (inEntry.getMethod() != JarEntry.STORED) {
                continue;
            }

            offset += JarFile.LOCHDR;

            JarEntry outEntry = new JarEntry(inEntry);
            outEntry.setTime(timestamp);

            outEntry.setComment(null);
            outEntry.setExtra(null);

            offset += outEntry.getName().length();

            int alignment = getStoredEntryDataAlignment(name, defaultAlignment);
            if (alignment > 0 && (offset % alignment != 0)) {
                int needed = alignment - (int) (offset % alignment);
                outEntry.setExtra(new byte[needed]);
                offset += needed;
            }

            out.putNextEntry(outEntry);
            byte[] buffer = new byte[BUFFER_LENGTH];
            try (InputStream data = in.getInputStream(inEntry)) {
                int num;
                while ((num = data.read(buffer)) > 0) {
                    out.write(buffer, 0, num);
                    offset += num;
                }
                out.flush();
            }
        }

        copyFilesExceptStoredFile(entryNames, in, out, timestamp);
    }

    private static void copyFilesExceptStoredFile(List<String> entryNames, JarFile in,
        JarOutputStream out, long timestamp) throws IOException {
        byte[] buffer = new byte[BUFFER_LENGTH];

        for (String name : entryNames) {
            JarEntry inEntry = in.getJarEntry(name);
            if (inEntry.getMethod() == JarEntry.STORED) {
                continue;
            }

            JarEntry outEntry = new JarEntry(name);
            outEntry.setTime(timestamp);
            out.putNextEntry(outEntry);

            try (InputStream data = in.getInputStream(inEntry);) {
                int num;
                while ((num = data.read(buffer)) > 0) {
                    out.write(buffer, 0, num);
                }
                out.flush();
            }
        }
    }

    /**
     * If store entry is end with '.so', use 4096-alignment, otherwise, use default-alignment.
     *
     * @param entryName name of entry
     * @param defaultAlignment default value of alignment.
     * @return value of alignment.
     */
    private static int getStoredEntryDataAlignment(String entryName, int defaultAlignment) {
        if (defaultAlignment <= 0) {
            return 0;
        }
        if (entryName.endsWith(".so")) {
            return STORED_ENTRY_SO_ALIGNMENT;
        }
        return defaultAlignment;
    }

    private static byte[] getHapSigningBlock(
            Set<ContentDigestAlgorithm> contentDigestAlgorithms,
            List<SigningBlock> optionalBlocks,
            SignerConfig signerConfig,
            ZipDataInput[] hapData)
        throws SignatureException {
        /**
         * Compute digests of Hap contents
         * Sign the digests and wrap the signature and signer info into the Hap Signing Block
         */
        byte[] hapSignatureBytes = null;
        try {
            Map<ContentDigestAlgorithm, byte[]> contentDigests =
                HapUtils.computeDigests(contentDigestAlgorithms, hapData, optionalBlocks);
            hapSignatureBytes = generateHapSigningBlock(signerConfig, contentDigests, optionalBlocks);
        } catch (DigestException | IOException e) {
            throw new SignatureException("Failed to compute digests of HAP", e);
        }
        return hapSignatureBytes;
    }

    private static byte[] generateHapSigningBlock(
            SignerConfig signerConfig,
            Map<ContentDigestAlgorithm, byte[]> contentDigests,
            List<SigningBlock> optionalBlocks)
            throws SignatureException {
        byte[] hapSignatureSchemeBlock = generateHapSignatureSchemeBlock(signerConfig, contentDigests);
        return generateHapSigningBlock(hapSignatureSchemeBlock, optionalBlocks, signerConfig.getCompatibleVersion());
    }

    private static byte[] generateHapSigningBlock(byte[] hapSignatureSchemeBlock,
        List<SigningBlock> optionalBlocks, int compatibleVersion) {
        // FORMAT:
        // Proof-of-Rotation pairs(optional):
        // uint32:type
        // uint32:length
        // uint32:offset

        // Property pairs(optional):
        // uint32:type
        // uint32:length
        // uint32:offset

        // Profile capability pairs(optional):
        // uint32:type
        // uint32:length
        // uint32:offset

        // length bytes : app signing pairs
        // uint32:type
        // uint32:length
        // uint32:offset

        // repeated ID-value pairs(reserved extensions):
        // length bytes : Proof-of-Rotation values
        // length bytes : property values
        // length bytes : profile capability values
        // length bytes : signature schema values

        // uint32: block count
        // uint64: size
        // uint128: magic
        // uint32: version
        long optionalBlockSize = 0L;
        for (SigningBlock optionalBlock : optionalBlocks) {
            optionalBlockSize += optionalBlock.getLength();
        }

        long resultSize =
                ((OPTIONAL_TYPE_SIZE + OPTIONAL_LENGTH_SIZE + OPTIONAL_OFFSET_SIZE) * (optionalBlocks.size() + 1))
                        + optionalBlockSize // optional pair
                        + hapSignatureSchemeBlock.length // App signing pairs
                        + BLOCK_COUNT // block count
                        + BLOCK_SIZE // size
                        + BLOCK_MAGIC // magic
                        + BLOCK_VERSION; // version
        if (resultSize > Integer.MAX_VALUE) {
            throw new IllegalArgumentException("HapSigningBlock out of range : " + resultSize);
        }
        ByteBuffer result = ByteBuffer.allocate((int) resultSize);
        result.order(ByteOrder.LITTLE_ENDIAN);

        Map<Integer, Integer> typeAndOffsetMap = new HashMap<Integer, Integer>();
        int currentOffset = ((OPTIONAL_TYPE_SIZE + OPTIONAL_LENGTH_SIZE +
                OPTIONAL_OFFSET_SIZE) * (optionalBlocks.size() + 1));
        int currentOffsetInBlockValue = 0;
        int blockValueSizes = (int) (optionalBlockSize + hapSignatureSchemeBlock.length);
        byte[] blockValues = new byte[blockValueSizes];

        for (SigningBlock optionalBlock : optionalBlocks) {
            System.arraycopy(
                    optionalBlock.getValue(), 0, blockValues, currentOffsetInBlockValue, optionalBlock.getLength());
            typeAndOffsetMap.put(optionalBlock.getType(), currentOffset);
            currentOffset += optionalBlock.getLength();
            currentOffsetInBlockValue += optionalBlock.getLength();
        }

        System.arraycopy(
                hapSignatureSchemeBlock, 0, blockValues, currentOffsetInBlockValue, hapSignatureSchemeBlock.length);
        typeAndOffsetMap.put(HapUtils.HAP_SIGNATURE_SCHEME_V1_BLOCK_ID, currentOffset);

        int offset = 0;
        for (SigningBlock optionalBlock : optionalBlocks) {
            result.putInt(optionalBlock.getType()); // type
            result.putInt(optionalBlock.getLength()); // length
            offset = typeAndOffsetMap.get(optionalBlock.getType());
            result.putInt(offset); // offset
        }
        result.putInt(HapUtils.HAP_SIGNATURE_SCHEME_V1_BLOCK_ID); // type
        result.putInt(hapSignatureSchemeBlock.length); // length
        offset = typeAndOffsetMap.get(HapUtils.HAP_SIGNATURE_SCHEME_V1_BLOCK_ID);
        result.putInt(offset); // offset

        result.put(blockValues);

        result.putInt(optionalBlocks.size() + 1); // Signing block count
        result.putLong(resultSize); // length of hap signing block
        result.put(HapUtils.getHapSigningBlockMagic(compatibleVersion)); // magic
        result.putInt(HapUtils.getHapSigningBlockVersion(compatibleVersion)); // version
        return result.array();
    }

    private static byte[] generateHapSignatureSchemeBlock(
            SignerConfig signerConfig, Map<ContentDigestAlgorithm, byte[]> contentDigests) throws SignatureException {
        byte[] signerBlock = null;
        try {
            signerBlock = generateSignerBlock(signerConfig, contentDigests);
        } catch (SignatureException e) {
            throw new SignatureException("generate SignerBlock failed", e);
        }
        return signerBlock;
    }

    private static byte[] generateSignerBlock(
            SignerConfig signerConfig, Map<ContentDigestAlgorithm, byte[]> contentDigests) throws SignatureException {
        String mode = signerConfig.getOptions().getString(Options.MODE);
        if (!("remoteSign".equalsIgnoreCase(mode)) && signerConfig.getCertificates().isEmpty()) {
                throw new SignatureException("No certificates configured for signer");
        }

        List<Pair<Integer, byte[]>> digests =
                new ArrayList<Pair<Integer, byte[]>>(signerConfig.getSignatureAlgorithms().size());
        for (SignatureAlgorithm signatureAlgorithm : signerConfig.getSignatureAlgorithms()) {
            ContentDigestAlgorithm contentDigestAlgorithm = signatureAlgorithm.getContentDigestAlgorithm();
            byte[] contentDigest = contentDigests.get(contentDigestAlgorithm);
            if (contentDigest == null) {
                throw new SignatureException(
                        contentDigestAlgorithm.getDigestAlgorithm()
                                + " content digest for "
                                + signatureAlgorithm.getSignatureAlgAndParams().getFirst()
                                + " not computed");
            }
            digests.add(Pair.create(signatureAlgorithm.getId(), contentDigest));
        }
        byte[] unsignedHapDigest = HapUtils.encodeListOfPairsToByteArray(digests);
        return Pkcs7Generator.BC.generateSignedData(unsignedHapDigest, signerConfig);
    }

    /**
     * Signs the provided Hap using Hap Signature Scheme and returns the
     * signed block as an array of ByteBuffer
     *
     * @param contents Hap content before ZIP CD
     * @param signerConfig signer config
     * @param optionalBlocks optional blocks
     * @return signed block
     * @throws SignatureException if an error occurs when sign hap file.
     */
    public static byte[] sign(ZipDataInput[] contents, SignerConfig signerConfig, List<SigningBlock> optionalBlocks)
        throws SignatureException {
        Set<ContentDigestAlgorithm> contentDigestAlgorithms = new HashSet<ContentDigestAlgorithm>();
        for (SignatureAlgorithm signatureAlgorithm : signerConfig.getSignatureAlgorithms()) {
            contentDigestAlgorithms.add(signatureAlgorithm.getContentDigestAlgorithm());
        }
        return getHapSigningBlock(contentDigestAlgorithms, optionalBlocks, signerConfig, contents);
    }
}
