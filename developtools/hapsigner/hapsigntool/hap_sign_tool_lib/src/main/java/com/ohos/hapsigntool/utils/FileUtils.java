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

package com.ohos.hapsigntool.utils;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.ohos.hapsigntool.error.ERROR;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.Closeable;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.nio.charset.Charset;
import java.nio.file.Files;

/**
 * Common file operation.
 *
 * @since 2021/12/28
 */
public final class FileUtils {

    /**
     * LOGGER.
     */
    private static final Logger LOGGER = LogManager.getLogger(FileUtils.class);
    /**
     * add GSON static.
     */
    public static final Gson GSON = (new GsonBuilder()).disableHtmlEscaping().create();
    /**
     * add GSON_PRETTY_PRINT static.
     */
    public static final Gson GSON_PRETTY_PRINT = (new GsonBuilder()).disableHtmlEscaping().setPrettyPrinting().create();
    /**
     * File reader block size
     */
    public static final int FILE_BUFFER_BLOCK = 4096;
    /**
     * File end
     */
    public static final int FILE_END = -1;
    /**
     * Expected split string length
     */
    public static final int SPLIT_LENGTH = 2;

    private static final byte[] EMPTY_BYTE_ARRAY = new byte[0];

    private FileUtils() {
    }

    /**
     * Close closeable quietly.
     *
     * @param closeable closeable
     */
    public static void close(Closeable closeable) {
        if (closeable != null) {
            try {
                closeable.close();
            } catch (IOException exception) {
                LOGGER.debug(exception.getMessage(), exception);
            }
        }
    }

    /**
     * Read byte from input file.
     *
     * @param file Which file to read
     * @return byte content
     * @throws IOException Read failed
     */
    public static byte[] readFile(File file) throws IOException {
        return read(new FileInputStream(file));
    }

    /**
     * Read byte from input stream.
     *
     * @param input Input stream
     * @return File content
     * @throws IOException Read failed
     */
    public static byte[] read(InputStream input) throws IOException {
        try (ByteArrayOutputStream output = new ByteArrayOutputStream()) {
            byte[] buffer = new byte[FILE_BUFFER_BLOCK];
            int read;
            while ((read = input.read(buffer)) != FILE_END) {
                output.write(buffer, 0, read);
            }
            return output.toByteArray();
        } finally {
            close(input);
        }
    }

    /**
     * Out put content to file.
     *
     * @param content Which content to out put
     * @param output  File to write
     * @throws IOException Write failed
     */
    public static void write(byte[] content, File output) throws IOException {
        try (FileOutputStream out = new FileOutputStream(output)) {
            for (byte con : content) {
                out.write(con);
            }
        }
    }

    /**
     * Check file exist or not.
     *
     * @param filePath File path
     * @return Is file exist
     */
    public static boolean isFileExist(String filePath) {
        return new File(filePath).exists();
    }

    /**
     * Throw runtime exception if not allowed file type.
     *
     * @param filePath file path
     * @param types    Such as "txt" "json" "mp3"
     */
    public static void validFileType(String filePath, String... types) {
        String suffix = getSuffix(filePath);
        ValidateUtils.throwIfNotMatches(!StringUtils.isEmpty(suffix),
                ERROR.NOT_SUPPORT_ERROR, "Not support file: " + filePath);
        boolean isMatches = false;
        for (String type : types) {
            if (StringUtils.isEmpty(type)) {
                continue;
            }
            if (type.equalsIgnoreCase(suffix)) {
                isMatches = true;
                break;
            }
        }
        ValidateUtils.throwIfNotMatches(isMatches, ERROR.NOT_SUPPORT_ERROR, "Not support file: " + filePath);
    }

    /**
     * Get suffix of file.
     *
     * @param filePath file path
     * @return file suffix. Such as "txt" "json" "p12"
     */
    public static String getSuffix(String filePath) {
        if (StringUtils.isEmpty(filePath)) {
            return "";
        }
        File file = new File(filePath);
        String fileName = file.getName();
        String[] temps = fileName.split("\\.");
        if (temps.length < SPLIT_LENGTH) {
            return "";
        }
        return temps[temps.length - 1];
    }

    /**
     * Write data in file to output stream
     *
     * @param file input file path.
     * @param dos output stream.
     * @return true, if write successfully.
     */
    public static boolean writeFileToDos(String file, DataOutputStream dos) {
        if (dos == null) {
            return false;
        }
        File src = new File(file);
        try (FileInputStream fileStream = new FileInputStream(src)) {
            int temp;
            byte[] buf = new byte[FILE_BUFFER_BLOCK];
            while ((temp = fileStream.read(buf)) > 0) {
                dos.write(buf, 0, temp);
            }
            return true;
        } catch (FileNotFoundException e) {
            LOGGER.error("Failed to get input stream object.");
        } catch (IOException e) {
            LOGGER.error("Failed to read or write data.");
        }
        return false;
    }

    /**
     * Write byte array to output stream
     *
     * @param data byte array
     * @param dos output stream
     * @return true, if write successfully.
     */
    public static boolean writeByteToDos(byte[] data, DataOutputStream dos) {
        try {
            dos.write(data);
        } catch (IOException e) {
            LOGGER.error("Faile to write data to output stream.");
            return false;
        }
        return true;
    }

    /**
     * Get length of file.
     *
     * @param filePath input file path.
     * @return long value of file length.
     */
    public static long getFileLen(String filePath) {
        File file = new File(filePath);
        if (file.exists() && file.isFile()) {
            return file.length();
        }
        return -1;
    }

    /**
     * Write byte array data to output file.
     *
     * @param signHeadByte byte array data.
     * @param outFile output file path.
     * @return true, if write successfully.
     */
    public static boolean writeByteToOutFile(byte[] signHeadByte, String outFile) {
        try (OutputStream ops = new FileOutputStream(outFile, true)) {
            ops.write(signHeadByte, 0, signHeadByte.length);
            ops.flush();
            return true;
        } catch (FileNotFoundException e) {
            LOGGER.error("Failed to get output stream object, outfile: " + outFile);
        } catch (IOException e) {
            LOGGER.error("Failed to write data to ops, outfile: " + outFile);
        }
        return false;
    }

    /**
     * Check input file is valid.
     *
     * @param file input file.
     * @throws IOException file is a directory or can't be read.
     */
    public static void isValidFile(File file) throws IOException {
        if (!file.exists()) {
            throw new FileNotFoundException("File '" + file + "' does not exist");
        }

        if (file.isDirectory()) {
            throw new IOException("File '" + file + "' exists but is a directory");
        }

        if (!file.canRead()) {
            throw new IOException("File '" + file + "' cannot be read");
        }
    }

    /**
     * Open an input stream of input file safely.
     * @param file input file.
     * @return an input stream of input file
     * @throws IOException file is a directory or can't be read.
     */
    public static FileInputStream openInputStream(File file) throws IOException {
        isValidFile(file);
        return new FileInputStream(file);
    }

    private static byte[] toByteArray(final InputStream input, final int size) throws IOException {
        if (size < 0) {
            throw new IllegalArgumentException("Size must be equal or greater than zero: " + size);
        }

        if (size == 0) {
            return EMPTY_BYTE_ARRAY;
        }

        final byte[] data = new byte[size];
        int offset = 0;
        int read;

        while (offset < size && (read = input.read(data, offset, size - offset)) != FILE_END) {
            offset += read;
        }

        if (offset != size) {
            throw new IOException("Unexpected read size. current: " + offset + ", expected: " + size);
        }

        return data;
    }

    /**
     * Read file to byte array.
     *
     * @param file input file.
     * @return byte array of file-content.
     * @throws IOException fill exception
     */
    public static byte[] readFileToByteArray(File file) throws IOException {
        try (InputStream in = openInputStream(file)) {
            final long fileLength = file.length();
            if (fileLength > Integer.MAX_VALUE) {
                throw new IllegalArgumentException("Size cannot be greater than Integer max value: " + fileLength);
            }
            return toByteArray(in, (int) fileLength);
        }
    }

    /**
     * Write a string to file
     *
     * @param source String will be written.
     * @param filePath path to write file.
     * @param charset a charset
     * @throws IOException write error.
     */
    public static void writeStringToFile(String source, String filePath, Charset charset) throws IOException {
        try (BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
                new FileOutputStream(filePath), charset))) {
            writer.write(source);
            writer.flush();
        }
    }

    /**
     * Delete a file quietly
     *
     * @param file the file to delete
     */
    public static void deleteFile(File file) {
        if (file != null && file.isFile()) {
            try {
                Files.delete(file.toPath());
            } catch (IOException e) {
                LOGGER.warn("delete file '{}' error, error message: {}", file, e.getMessage());
            }
        }
    }
}
