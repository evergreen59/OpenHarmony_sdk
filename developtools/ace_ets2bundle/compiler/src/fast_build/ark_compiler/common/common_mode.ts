/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import fs from 'fs';
import path from 'path';

import {
  HASH_FILE_NAME,
  IS_CACHE_INVALID,
  GEN_ABC_PLUGIN_NAME,
  GEN_ABC_SCRIPT,
  blue,
  reset
} from './ark_define';
import { initArkConfig } from './process_ark_config';
import {
  nodeLargeOrEqualTargetVersion,
  mkdirsSync,
  validateFilePathLength
} from '../../../utils';
import {
  isEs2Abc,
  isOhModules,
  isTs2Abc
} from '../../../ark_utils';
import {
  genTemporaryModuleCacheDirectoryForBundle
} from '../utils';

export abstract class CommonMode {
  projectConfig: any;
  arkConfig: any;
  cmdArgs: string[] = [];
  logger: any;
  throwArkTsCompilerError: any;
  hashJsonFilePath: string;
  genAbcScriptPath: string;
  triggerAsync: any;
  triggerEndSignal: any;

  constructor(rollupObject: any) {
    this.projectConfig = Object.assign(rollupObject.share.arkProjectConfig, rollupObject.share.projectConfig);
    this.arkConfig = initArkConfig(this.projectConfig);
    this.cmdArgs = this.initCmdEnv();
    this.logger = rollupObject.share.getLogger(GEN_ABC_PLUGIN_NAME);
    this.throwArkTsCompilerError = rollupObject.share.throwArkTsCompilerError;
    this.hashJsonFilePath = this.genHashJsonFilePath();
    this.genAbcScriptPath = path.resolve(__dirname, GEN_ABC_SCRIPT);
    // Each time triggerAsync() was called, IDE will wait for asynchronous operation to finish before continue excuting.
    // The finish signal was passed to IDE by calling triggerEndSignal() in the child process created by triggerAsync()
    // When multiple workers were invoked by triggerAsync(), IDE will wait until the times of calling
    // triggerEndSignal() matches the number of workers invoked.
    // If the child process throws an error by calling throwArkTsCompilerError(), IDE will reset the counting state.
    this.triggerAsync = rollupObject.async;
    this.triggerEndSignal = rollupObject.signal;
  }

  private initCmdEnv() {
    let args: string[] = [];

    if (isTs2Abc(this.projectConfig)) {
      let ts2abc: string = this.arkConfig.ts2abcPath;
      validateFilePathLength(ts2abc, this.logger);

      ts2abc = '"' + ts2abc + '"';
      args = [`${this.arkConfig.nodePath}`, '--expose-gc', ts2abc];
      if (this.arkConfig.isDebug) {
        args.push('--debug');
      }
      if (isOhModules(this.projectConfig)) {
        args.push('--oh-modules');
      }
    } else if (isEs2Abc(this.projectConfig)) {
      const es2abc: string = this.arkConfig.es2abcPath;
      validateFilePathLength(es2abc, this.logger);

      args = ['"' + es2abc + '"'];
      if (this.arkConfig.isDebug) {
        args.push('--debug-info');
      }
    } else {
      this.throwArkTsCompilerError('ArkTS:ERROR please set panda mode');
    }

    return args;
  }

  private genHashJsonFilePath() {
    if (this.projectConfig.cachePath) {
      if (!fs.existsSync(this.projectConfig.cachePath) || !fs.statSync(this.projectConfig.cachePath).isDirectory()) {
        this.logger.debug(blue, `ArkTS:WARN cache path does bit exist or is not directory`, reset);
        return '';
      }
      const hashJsonPath: string = path.join(genTemporaryModuleCacheDirectoryForBundle(this.projectConfig), HASH_FILE_NAME);
      validateFilePathLength(hashJsonPath, this.logger);
      mkdirsSync(path.dirname(hashJsonPath));
      return hashJsonPath;
    } else {
      this.logger.debug(blue, `ArkTS:WARN cache path not specified`, reset);
      return '';
    }
  }

  setupCluster(cluster: any): void {
    cluster.removeAllListeners('exit');
    if (nodeLargeOrEqualTargetVersion(16)) {
      cluster.setupPrimary({
        exec: this.genAbcScriptPath,
        windowsHide: true
      });
    } else {
      cluster.setupMaster({
        exec: this.genAbcScriptPath,
        windowsHide: true
      });
    }
  }

  protected needRemoveCacheInfo(rollupObject: any): boolean {
    return rollupObject.cache.get(IS_CACHE_INVALID) || rollupObject.cache.get(IS_CACHE_INVALID) === undefined;
  }

  protected removeCacheInfo(rollupObject: any): void {
    if (this.needRemoveCacheInfo(rollupObject)) {
      this.removeCompilationCache();
    }
    rollupObject.cache.set(IS_CACHE_INVALID, false);
  }

  abstract removeCompilationCache(): void;
}
