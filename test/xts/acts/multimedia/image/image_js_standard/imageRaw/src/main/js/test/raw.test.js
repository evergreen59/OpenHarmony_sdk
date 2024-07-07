/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import image from "@ohos.multimedia.image";
import fileio from "@ohos.fileio";
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from "@ohos/hypium";
import featureAbility from "@ohos.ability.featureAbility";
export default function imageRaw() {
    describe("imageRaw", function () {
        const RGBA_8888 = image.PixelMapFormat.RGBA_8888;
        let filePath;
        let fdNumber;
        async function getFd(fileName) {
            let context = await featureAbility.getContext();
            await context.getFilesDir().then((data) => {
                filePath = data + "/" + fileName;
                console.info("image case filePath is " + filePath);
            });
            await fileio
                .open(filePath, 0o2, 0o777)
                .then(
                    (data) => {
                        fdNumber = data;
                        console.info("image case open fd success " + fdNumber);
                    },
                    (err) => {
                        console.info("image cese open fd fail" + err);
                    }
                )
                .catch((err) => {
                    console.info("image case open fd err " + err);
                });
        }

        beforeAll(async function () {
            console.info("beforeAll case");
        });

        beforeEach(function () {
            console.info("beforeEach case");
        });

        afterEach(async function () {
            console.info("afterEach case");
        });

        afterAll(async function () {
            console.info("afterAll case");
        });

        async function packingPromise(done, testNum, pixelmap, packOpts) {
            console.info(`${testNum} packingPromise enter`);
            try {
                const imagePackerApi = image.createImagePacker();
                if (imagePackerApi == undefined) {
                    console.info(`${testNum} packingPromise create image packer failed`);
                    expect(false).assertTrue();
                    done();
                } else {
                    let packOptsFormat = `format:` + packOpts.format;
                    let packOptsQuality = `quality:` + packOpts.quality;
                    console.info(
                        `${testNum} packingPromise packOpts={` + packOptsFormat + `, ` + packOptsQuality + `}`
                    );
                    imagePackerApi
                        .packing(pixelmap, packOpts)
                        .then((data) => {
                            console.info(`${testNum} packing finished`);
                            if (data != undefined) {
                                console.info(`${testNum} packing success`);
                                var dataArr = new Uint8Array(data);
                                console.info(`${testNum} packing show begin(length:` + dataArr.length + `)`);
                                var line = 0;
                                for (var i = 0; i < dataArr.length; i++) {
                                    var str = `dataArr[` + i + `]=`;
                                    for (var j = 0; j < 20 && i < dataArr.length; j++, i++) {
                                        str = str + "," + dataArr[i];
                                    }
                                    console.info(`${testNum} packing ` + str);
                                    i--;
                                    line++;
                                }
                                console.info(`${testNum} packing show end(line:` + line + `)`);
                                expect(true).assertTrue();
                                done();
                            } else {
                                console.info(`${testNum} packing failed`);
                                expect(false).assertTrue();
                                done();
                            }
                        })
                        .catch((error) => {
                            console.log(`${testNum} packing error: ` + error);
                            expect(false).assertTrue();
                            done();
                        });
                }
            } catch (error) {
                console.info(`${testNum} packingPromise error: ` + error);
                expect(false).assertTrue();
                done();
            }
            console.info(`${testNum} packingPromise leave`);
        }

        async function createPixelMapPromise(done, testNum, picName, decodeOpts, packFunc, packOpts) {
            let imageSourceApi;
            try {
                await getFd(picName);
                imageSourceApi = image.createImageSource(fdNumber);
                if (imageSourceApi == undefined) {
                    console.info(`${testNum} createPixelMapPromise create imagesource failed`);
                    expect(false).assertTrue();
                    done();
                } else {
                    console.info(`${testNum} createPixelMapPromise create imagesource success`);
                    imageSourceApi
                        .createPixelMap(decodeOpts)
                        .then((pixelmap) => {
                            if (pixelmap != undefined) {
                                console.info(`${testNum} createPixelMap create pixelmap success`);
                                packFunc(done, testNum, pixelmap, packOpts);
                            } else {
                                console.info(`${testNum} createPixelMap create pixelmap failed`);
                                expect(false).assertTrue();
                                done();
                            }
                        })
                        .catch((error) => {
                            console.log(`${testNum} createPixelMap err: ` + error);
                            expect(false).assertTrue();
                            done();
                        });
                }
            } catch (error) {
                console.info(`${testNum}  error: ` + error);
                expect(false).assertTrue();
                done();
            }
            console.info(`${testNum} createPixelMapPromise leave`);
        }

        async function createPixelMapCallBack(done, testNum, picName, decodeOpts, packFunc, packOpts) {
            let imageSourceApi;
            try {
                await getFd(picName);
                imageSourceApi = image.createImageSource(fdNumber);
                if (imageSourceApi == undefined) {
                    console.info(`${testNum} createPixelMapPromise create imagesource failed`);
                    expect(false).assertTrue();
                    done();
                } else {
                    console.info(`${testNum} createPixelMapPromise create imagesource success`);
                    imageSourceApi.createPixelMap(decodeOpts, (err, pixelmap) => {
                        if (pixelmap != undefined) {
                            console.info(`${testNum} createPixelMap create pixelmap success`);
                            packFunc(done, testNum, pixelmap, packOpts);
                        } else {
                            console.info(`${testNum} createPixelMap create pixelmap failed err: ${err}`);
                            expect(false).assertTrue();
                            done();
                        }
                    });
                }
            } catch (error) {
                console.info(`${testNum}  error: ` + error);
                expect(false).assertTrue();
                done();
            }
            console.info(`${testNum} createPixelMapPromise leave`);
        }

        async function createPixelMapCallBackErr(done, testNum, picName, decodeOpts) {
            let imageSourceApi;
            try {
                await getFd(picName);
                imageSourceApi = image.createImageSource(fdNumber);
                if (imageSourceApi == undefined) {
                    console.info(`${testNum} createPixelMapPromise create imagesource failed`);
                    expect(false).assertTrue();
                    done();
                } else {
                    console.info(`${testNum} createPixelMapPromise create imagesource success`);
                    imageSourceApi.createPixelMap(decodeOpts, (err, pixelmap) => {
                        if (pixelmap == undefined) {
                            expect(true).assertTrue();
                            console.info(`${testNum} success: ` + err);
                            done();
                        } else {
                            expect(false).assertTrue();
                            done();
                        }
                    });
                }
            } catch (error) {
                console.info(`${testNum}  error: ` + error);
                expect(false).assertTrue();
                done();
            }
            console.info(`${testNum} createPixelMap leave`);
        }
        async function createPixelMapPromiseErr(done, testNum, picName, decodeOpts) {
            let imageSourceApi;
            try {
                await getFd(picName);
                imageSourceApi = image.createImageSource(fdNumber);
                if (imageSourceApi == undefined) {
                    console.info(`${testNum} createPixelMapPromise create imagesource failed`);
                    expect(false).assertTrue();
                    done();
                } else {
                    console.info(`${testNum} createPixelMapPromise create imagesource success`);
                    imageSourceApi
                        .createPixelMap(decodeOpts)
                        .then((pixelmap) => {
                            console.log(`${testNum} failed`);
                            expect(false).assertTrue();
                            done();
                        })
                        .catch((error) => {
                            console.log(`${testNum} createPixelMap err: ` + error);
                            expect(true).assertTrue();
                            done();
                        });
                }
            } catch (error) {
                console.info(`${testNum}  error: ` + error);
                expect(false).assertTrue();
                done();
            }
            console.info(`${testNum} createPixelMap leave`);
        }

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_0100
         * @tc.name      : createPixelMap - promise-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 0
         */
        it("SUB_GRAPHIC_IMAGE_RAW_0100", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 192, height: 108 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapPromise(done, "SUB_GRAPHIC_IMAGE_RAW_0100", "test.arw", decodeOpts, packingPromise, packOpts);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_0200
         * @tc.name      : createPixelMap - promise-cr2
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 1
         */
        it("SUB_GRAPHIC_IMAGE_RAW_0200", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapPromise(done, "SUB_GRAPHIC_IMAGE_RAW_0200", "test.cr2", decodeOpts, packingPromise, packOpts);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_0300
         * @tc.name      : createPixelMap - promise-dng
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_0300", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapPromise(
                done,
                "SUB_GRAPHIC_IMAGE_RAW_0300",
                "test_dng.dng",
                decodeOpts,
                packingPromise,
                packOpts
            );
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_0400
         * @tc.name      : createPixelMap - promise-nrw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_0400", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapPromise(done, "SUB_GRAPHIC_IMAGE_RAW_0400", "test.nrw", decodeOpts, packingPromise, packOpts);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_0500
         * @tc.name      : createPixelMap - promise-pef
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_0500", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapPromise(done, "SUB_GRAPHIC_IMAGE_RAW_0500", "test.pef", decodeOpts, packingPromise, packOpts);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_0600
         * @tc.name      : createPixelMap - promise-raf
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_0600", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapPromise(done, "SUB_GRAPHIC_IMAGE_RAW_0600", "test.raf", decodeOpts, packingPromise, packOpts);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_0700
         * @tc.name      : createPixelMap - promise-rw2
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_0700", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapPromise(done, "SUB_GRAPHIC_IMAGE_RAW_0700", "test.rw2", decodeOpts, packingPromise, packOpts);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_0800
         * @tc.name      : createPixelMap - callback-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 0
         */
        it("SUB_GRAPHIC_IMAGE_RAW_0800", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 192, height: 108 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapCallBack(
                done,
                "SUB_GRAPHIC_IMAGE_RAW_0800",
                "test.arw",
                decodeOpts,
                packingPromise,
                packOpts
            );
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_0900
         * @tc.name      : createPixelMap - callback-cr2
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 1
         */
        it("SUB_GRAPHIC_IMAGE_RAW_0900", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapCallBack(
                done,
                "SUB_GRAPHIC_IMAGE_RAW_0900",
                "test.cr2",
                decodeOpts,
                packingPromise,
                packOpts
            );
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1000
         * @tc.name      : createPixelMap - callback-dng
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1000", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapCallBack(
                done,
                "SUB_GRAPHIC_IMAGE_RAW_1000",
                "test_dng.dng",
                decodeOpts,
                packingPromise,
                packOpts
            );
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1100
         * @tc.name      : createPixelMap - callback-nrw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1100", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapCallBack(
                done,
                "SUB_GRAPHIC_IMAGE_RAW_1100",
                "test.nrw",
                decodeOpts,
                packingPromise,
                packOpts
            );
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1200
         * @tc.name      : createPixelMap - callback-pef
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1200", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapCallBack(
                done,
                "SUB_GRAPHIC_IMAGE_RAW_1200",
                "test.pef",
                decodeOpts,
                packingPromise,
                packOpts
            );
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1300
         * @tc.name      : createPixelMap - callback-raf
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1300", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapCallBack(
                done,
                "SUB_GRAPHIC_IMAGE_RAW_1300",
                "test.raf",
                decodeOpts,
                packingPromise,
                packOpts
            );
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1400
         * @tc.name      : createPixelMap - callback-rw2
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         *                 4.packing
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 2
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1400", 0, async function (done) {
            let decodeOpts = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 160, height: 120 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                index: 0,
            };
            let packOpts = { format: ["image/webp"], quality: 100 };
            createPixelMapCallBack(
                done,
                "SUB_GRAPHIC_IMAGE_RAW_1400",
                "test.rw2",
                decodeOpts,
                packingPromise,
                packOpts
            );
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1500
         * @tc.name      : createPixelMap - callback-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1500", 0, async function (done) {
            let decodingOption = {
                sampleSize: -1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index: 0,
            };
            await createPixelMapCallBackErr(done, "SUB_GRAPHIC_IMAGE_RAW_1500", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1600
         * @tc.name      : createPixelMap - callback-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1600", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index: -1,
            };
            await createPixelMapCallBackErr(done, "SUB_GRAPHIC_IMAGE_RAW_1600", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1700
         * @tc.name      : createPixelMap - callback-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1700", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 500,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index: 0,
            };
            await createPixelMapCallBackErr(done, "SUB_GRAPHIC_IMAGE_RAW_1700", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1800
         * @tc.name      : createPixelMap - callback-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1800", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: false,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: 33,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index: 0,
            };
            await createPixelMapCallBackErr(done, "SUB_GRAPHIC_IMAGE_RAW_1800", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_1900
         * @tc.name      : createPixelMap - callback-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_1900", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 10000, y: 0 },
                index: 0,
            };
            await createPixelMapCallBackErr(done, "SUB_GRAPHIC_IMAGE_RAW_1900", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_2000
         * @tc.name      : createPixelMap - callback-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_2000", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 10000 },
                index: 0,
            };
            await createPixelMapCallBackErr(done, "SUB_GRAPHIC_IMAGE_RAW_2000", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_2100
         * @tc.name      : createPixelMap - promise-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_2100", 0, async function (done) {
            let decodingOption = {
                sampleSize: -1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index: 0,
            };
            await createPixelMapPromiseErr(done, "SUB_GRAPHIC_IMAGE_RAW_2100", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_2200
         * @tc.name      : createPixelMap - promise-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_2200", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index: -1,
            };
            await createPixelMapPromiseErr(done, "SUB_GRAPHIC_IMAGE_RAW_2200", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_2300
         * @tc.name      : createPixelMap - promise-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_2300", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 500,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index: 0,
            };
            await createPixelMapPromiseErr(done, "SUB_GRAPHIC_IMAGE_RAW_2300", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_2400
         * @tc.name      : createPixelMap - promise-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_2400", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: false,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: 33,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index: 0,
            };
            await createPixelMapPromiseErr(done, "SUB_GRAPHIC_IMAGE_RAW_2400", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_2500
         * @tc.name      : createPixelMap - promise-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_2500", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 10000, y: 0 },
                index: 0,
            };
            await createPixelMapPromiseErr(done, "SUB_GRAPHIC_IMAGE_RAW_2500", "test.arw", decodingOption);
        });

        /**
         * @tc.number    : SUB_GRAPHIC_IMAGE_RAW_2600
         * @tc.name      : createPixelMap - promise-arw
         * @tc.desc      : 1.create imagesource
         *                 2.set DecodeOptions
         *                 3.create PixelMap
         * @tc.size      : MEDIUM
         * @tc.type      : Functional
         * @tc.level     : Level 3
         */
        it("SUB_GRAPHIC_IMAGE_RAW_2600", 0, async function (done) {
            let decodingOption = {
                sampleSize: 1,
                editable: true,
                desiredSize: { width: 1, height: 2 },
                rotate: 0,
                desiredPixelFormat: RGBA_8888,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 10000 },
                index: 0,
            };
            await createPixelMapPromiseErr(done, "SUB_GRAPHIC_IMAGE_RAW_2600", "test.arw", decodingOption);
        });
    });
}
