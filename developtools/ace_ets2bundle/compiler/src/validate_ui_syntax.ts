/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import ts from 'typescript';
import path from 'path';
import fs from 'fs';

import {
  INNER_COMPONENT_DECORATORS,
  COMPONENT_DECORATOR_ENTRY,
  COMPONENT_DECORATOR_PREVIEW,
  COMPONENT_DECORATOR_COMPONENT,
  COMPONENT_DECORATOR_CUSTOM_DIALOG,
  NATIVE_MODULE,
  SYSTEM_PLUGIN,
  OHOS_PLUGIN,
  INNER_COMPONENT_MEMBER_DECORATORS,
  COMPONENT_FOREACH,
  COMPONENT_LAZYFOREACH,
  COMPONENT_STATE_DECORATOR,
  COMPONENT_LINK_DECORATOR,
  COMPONENT_PROP_DECORATOR,
  COMPONENT_STORAGE_PROP_DECORATOR,
  COMPONENT_STORAGE_LINK_DECORATOR,
  COMPONENT_PROVIDE_DECORATOR,
  COMPONENT_CONSUME_DECORATOR,
  COMPONENT_OBJECT_LINK_DECORATOR,
  COMPONENT_OBSERVED_DECORATOR,
  COMPONENT_LOCAL_STORAGE_LINK_DECORATOR,
  COMPONENT_LOCAL_STORAGE_PROP_DECORATOR,
  STYLES,
  VALIDATE_MODULE,
  COMPONENT_BUILDER_DECORATOR,
  COMPONENT_CONCURRENT_DECORATOR,
  COMPONENT_EXTEND_DECORATOR,
  COMPONENT_STYLES_DECORATOR,
  RESOURCE_NAME_TYPE,
  TTOGGLE_CHECKBOX,
  TOGGLE_SWITCH,
  COMPONENT_BUTTON,
  COMPONENT_TOGGLE,
  COMPONENT_BUILDERPARAM_DECORATOR,
  ESMODULE,
  CARD_ENABLE_DECORATORS,
  CARD_LOG_TYPE_DECORATORS,
  JSBUNDLE
} from './pre_define';
import {
  INNER_COMPONENT_NAMES,
  AUTOMIC_COMPONENT,
  SINGLE_CHILD_COMPONENT,
  SPECIFIC_CHILD_COMPONENT,
  BUILDIN_STYLE_NAMES,
  EXTEND_ATTRIBUTE,
  GLOBAL_STYLE_FUNCTION,
  STYLES_ATTRIBUTE,
  CUSTOM_BUILDER_METHOD,
  GLOBAL_CUSTOM_BUILDER_METHOD,
  INNER_CUSTOM_BUILDER_METHOD,
  INNER_STYLE_FUNCTION
} from './component_map';
import {
  LogType,
  LogInfo,
  componentInfo,
  addLog,
  hasDecorator,
  storedFileInfo
} from './utils';
import { getPackageInfo } from './ark_utils'
import { projectConfig, abilityPagesFullPath } from '../main';
import {
  collectExtend,
  isExtendFunction,
  transformLog,
  validatorCard
} from './process_ui_syntax';
import { logger } from './compile_info';

export interface ComponentCollection {
  localStorageName: string;
  entryComponentPos: number;
  entryComponent: string;
  previewComponent: Array<string>;
  customDialogs: Set<string>;
  customComponents: Set<string>;
  currentClassName: string;
}

export interface IComponentSet {
  properties: Set<string>;
  regulars: Set<string>;
  states: Set<string>;
  links: Set<string>;
  props: Set<string>;
  storageProps: Set<string>;
  storageLinks: Set<string>;
  provides: Set<string>;
  consumes: Set<string>;
  objectLinks: Set<string>;
  localStorageLink: Map<string, Set<string>>;
  localStorageProp: Map<string, Set<string>>;
  builderParams: Set<string>;
  builderParamData: Set<string>;
}

export const componentCollection: ComponentCollection = {
  localStorageName: null,
  entryComponentPos: null,
  entryComponent: null,
  previewComponent: new Array(),
  customDialogs: new Set([]),
  customComponents: new Set([]),
  currentClassName: null
};

export const observedClassCollection: Set<string> = new Set();
export const enumCollection: Set<string> = new Set();
export const classMethodCollection: Map<string, Set<string>> = new Map();
export const dollarCollection: Set<string> = new Set();

export const propertyCollection: Map<string, Set<string>> = new Map();
export const stateCollection: Map<string, Set<string>> = new Map();
export const linkCollection: Map<string, Set<string>> = new Map();
export const propCollection: Map<string, Set<string>> = new Map();
export const regularCollection: Map<string, Set<string>> = new Map();
export const storagePropCollection: Map<string, Set<string>> = new Map();
export const storageLinkCollection: Map<string, Set<string>> = new Map();
export const provideCollection: Map<string, Set<string>> = new Map();
export const consumeCollection: Map<string, Set<string>> = new Map();
export const objectLinkCollection: Map<string, Set<string>> = new Map();
export const builderParamObjectCollection: Map<string, Set<string>> = new Map();
export const localStorageLinkCollection: Map<string, Map<string, Set<string>>> = new Map();
export const localStoragePropCollection: Map<string, Map<string, Set<string>>> = new Map();
export const builderParamInitialization: Map<string, Set<string>> = new Map();

export const isStaticViewCollection: Map<string, boolean> = new Map();

export const useOSFiles: Set<string> = new Set();
export const sourcemapNamesCollection: Map<string, Map<string, string>> = new Map();
export const originalImportNamesMap: Map<string, string> = new Map();

export function validateUISyntax(source: string, content: string, filePath: string,
  fileQuery: string): LogInfo[] {
  let log: LogInfo[] = [];
  if (process.env.compileMode === 'moduleJson' ||
    path.resolve(filePath) !== path.resolve(projectConfig.projectPath || '', 'app.ets')) {
    const res: LogInfo[] = checkComponentDecorator(source, filePath, fileQuery);
    if (res) {
      log = log.concat(res);
    }
    const allComponentNames: Set<string> =
      new Set([...INNER_COMPONENT_NAMES, ...componentCollection.customComponents]);
    checkUISyntax(filePath, allComponentNames, content, log);
    componentCollection.customComponents.forEach(item => componentInfo.componentNames.add(item));
  }

  return log;
}

function checkComponentDecorator(source: string, filePath: string,
  fileQuery: string): LogInfo[] | null {
  const log: LogInfo[] = [];
  const sourceFile: ts.SourceFile = ts.createSourceFile(filePath, source,
    ts.ScriptTarget.Latest, true, ts.ScriptKind.ETS);
  if (sourceFile && sourceFile.statements && sourceFile.statements.length) {
    const result: DecoratorResult = {
      entryCount: 0,
      previewCount: 0
    };
    sourceFile.statements.forEach((item, index, arr) => {
      if (isObservedClass(item)) {
        // @ts-ignore
        observedClassCollection.add(item.name.getText());
      }
      if (ts.isEnumDeclaration(item) && item.name) {
        enumCollection.add(item.name.getText());
      }
      if (ts.isStructDeclaration(item)) {
        if (item.name && ts.isIdentifier(item.name)) {
          if (item.decorators && item.decorators.length) {
            checkDecorators(item.decorators, result, item.name, log, sourceFile, item);
          } else {
            const message: string = `A struct should use decorator '@Component'.`;
            addLog(LogType.WARN, message, item.getStart(), log, sourceFile);
          }
        } else {
          const message: string = `A struct must have a name.`;
          addLog(LogType.ERROR, message, item.getStart(), log, sourceFile);
        }
      }
      if (ts.isMissingDeclaration(item)) {
        const decorators: ts.NodeArray<ts.Decorator> = item.decorators;
        for (let i = 0; i < decorators.length; i++) {
          if (decorators[i] && /struct/.test(decorators[i].getText())) {
            const message: string = `Please use a valid decorator.`;
            addLog(LogType.ERROR, message, item.getStart(), log, sourceFile);
            break;
          }
        }
      }
    });
    if (process.env.compileTool === 'rollup') {
      if (result.entryCount > 0) {
        storedFileInfo.wholeFileInfo[path.resolve(sourceFile.fileName)].hasEntry = true;
      } else {
        storedFileInfo.wholeFileInfo[path.resolve(sourceFile.fileName)].hasEntry = false;
      }
    }
    validateEntryAndPreviewCount(result, fileQuery, sourceFile.fileName, projectConfig.isPreview,
      !!projectConfig.checkEntry, log);
  }

  return log.length ? log : null;
}

function validateEntryAndPreviewCount(result: DecoratorResult, fileQuery: string,
  fileName: string, isPreview: boolean, checkEntry: boolean, log: LogInfo[]): void {
  if (result.previewCount > 10 && fileQuery === '?entry') {
    log.push({
      type: LogType.ERROR,
      message: `A page can contain at most 10 '@Preview' decorators.`,
      fileName: fileName
    });
  }
  if (result.entryCount > 1 && fileQuery === '?entry') {
    log.push({
      type: LogType.ERROR,
      message: `A page can't contain more than one '@Entry' decorator`,
      fileName: fileName
    });
  }
  if (isPreview && !checkEntry && result.previewCount < 1 && result.entryCount !== 1 &&
    fileQuery === '?entry') {
    log.push({
      type: LogType.ERROR,
      message: `A page which is being previewed must have one and only one '@Entry' `
        + `decorator, or at least one '@Preview' decorator.`,
      fileName: fileName
    });
  } else if ((!isPreview || isPreview && checkEntry) && result.entryCount !== 1 && fileQuery === '?entry' &&
    !abilityPagesFullPath.includes(path.resolve(fileName).toLowerCase())) {
    log.push({
      type: LogType.ERROR,
      message: `A page configured in '${projectConfig.pagesJsonFileName}' must have one and only one '@Entry' `
        + `decorator.`,
      fileName: fileName
    });
  }
}

export function isObservedClass(node: ts.Node): boolean {
  if (ts.isClassDeclaration(node) && hasDecorator(node, COMPONENT_OBSERVED_DECORATOR)) {
    return true;
  }
  return false;
}

export function isCustomDialogClass(node: ts.Node): boolean {
  if (ts.isClassDeclaration(node) && hasDecorator(node, COMPONENT_DECORATOR_CUSTOM_DIALOG)) {
    return true;
  }
  return false;
}

interface DecoratorResult {
  entryCount: number;
  previewCount: number;
}

function checkDecorators(decorators: ts.NodeArray<ts.Decorator>, result: DecoratorResult,
  component: ts.Identifier, log: LogInfo[], sourceFile: ts.SourceFile, node: ts.StructDeclaration): void {
  let hasComponentDecorator: boolean = false;
  const componentName: string = component.getText();
  decorators.forEach((element) => {
    const name: string = element.getText().replace(/\([^\(\)]*\)/, '').trim();
    if (INNER_COMPONENT_DECORATORS.has(name)) {
      componentCollection.customComponents.add(componentName);
      switch (name) {
        case COMPONENT_DECORATOR_ENTRY:
          checkEntryComponent(node, log, sourceFile);
          result.entryCount++;
          componentCollection.entryComponent = componentName;
          componentCollection.entryComponentPos = node.getStart();
          collectLocalStorageName(element);
          break;
        case COMPONENT_DECORATOR_PREVIEW:
          result.previewCount++;
          componentCollection.previewComponent.push(componentName);
          break;
        case COMPONENT_DECORATOR_COMPONENT:
          hasComponentDecorator = true;
          break;
        case COMPONENT_DECORATOR_CUSTOM_DIALOG:
          componentCollection.customDialogs.add(componentName);
          hasComponentDecorator = true;
          break;
      }
    } else {
      const pos: number = element.expression ? element.expression.pos : element.pos;
      const message: string = `The struct '${componentName}' use invalid decorator.`;
      addLog(LogType.WARN, message, pos, log, sourceFile);
    }
  });
  if (!hasComponentDecorator) {
    const message: string = `The struct '${componentName}' should use decorator '@Component'.`;
    addLog(LogType.WARN, message, component.pos, log, sourceFile);
  }
  if (BUILDIN_STYLE_NAMES.has(componentName)) {
    const message: string = `The struct '${componentName}' cannot have the same name ` +
      `as the built-in attribute '${componentName}'.`;
    addLog(LogType.ERROR, message, component.pos, log, sourceFile);
  }
  if (INNER_COMPONENT_NAMES.has(componentName)) {
    const message: string = `The struct '${componentName}' cannot have the same name ` +
      `as the built-in component '${componentName}'.`;
    addLog(LogType.ERROR, message, component.pos, log, sourceFile);
  }
}

function checkConcurrentDecorator(node: ts.FunctionDeclaration | ts.MethodDeclaration, log: LogInfo[],
  sourceFile: ts.SourceFile): void {
  if (projectConfig.compileMode === JSBUNDLE) {
    const message: string = `@Concurrent can only be used in ESMODULE compile mode.`;
    addLog(LogType.ERROR, message, node.decorators!.pos, log, sourceFile);
  }
  if (ts.isMethodDeclaration(node)) {
    const message: string = `@Concurrent can not be used on method. please use it on function declaration.`;
    addLog(LogType.ERROR, message, node.decorators!.pos, log, sourceFile);
  }
  if (node.asteriskToken) {
    let hasAsync: boolean = false;
    const checkAsyncModifier = (modifier: ts.Modifier) => modifier.kind === ts.SyntaxKind.AsyncKeyword;
    node.modifiers && (hasAsync = node.modifiers.some(checkAsyncModifier));
    const funcKind: string = hasAsync ? 'Async generator' : 'Generator';
    const message: string = `@Concurrent can not be used on ${funcKind} function declaration.`;
    addLog(LogType.ERROR, message, node.decorators!.pos, log, sourceFile);
  }
}

function collectLocalStorageName(node: ts.Decorator): void {
  if (node && node.expression && ts.isCallExpression(node.expression)) {
    if (node.expression.arguments && node.expression.arguments.length) {
      node.expression.arguments.forEach((item: ts.Node, index: number) => {
        if (ts.isIdentifier(item) && index === 0) {
          componentCollection.localStorageName = item.getText();
        }
      });
    }
  } else {
    componentCollection.localStorageName = null;
  }
}

function checkUISyntax(filePath: string, allComponentNames: Set<string>, content: string,
  log: LogInfo[]): void {
  const sourceFile: ts.SourceFile = ts.createSourceFile(filePath, content,
    ts.ScriptTarget.Latest, true, ts.ScriptKind.ETS);
  visitAllNode(sourceFile, sourceFile, allComponentNames, log);
}

function visitAllNode(node: ts.Node, sourceFileNode: ts.SourceFile, allComponentNames: Set<string>,
  log: LogInfo[]) {
  if (ts.isStructDeclaration(node) && node.name && ts.isIdentifier(node.name)) {
    collectComponentProps(node);
  }
  if (ts.isMethodDeclaration(node) || ts.isFunctionDeclaration(node)) {
    if (hasDecorator(node, COMPONENT_BUILDER_DECORATOR)) {
      CUSTOM_BUILDER_METHOD.add(node.name.getText());
      if (ts.isFunctionDeclaration(node)) {
        GLOBAL_CUSTOM_BUILDER_METHOD.add(node.name.getText());
      } else {
        INNER_CUSTOM_BUILDER_METHOD.add(node.name.getText());
      }
    } else if (ts.isFunctionDeclaration(node) && isExtendFunction(node)) {
      const componentName: string = isExtendFunction(node);
      collectExtend(EXTEND_ATTRIBUTE, componentName, node.name.getText());
    } else if (hasDecorator(node, COMPONENT_STYLES_DECORATOR)) {
      if (ts.isBlock(node.body) && node.body.statements && node.body.statements.length) {
        if (ts.isFunctionDeclaration(node)) {
          GLOBAL_STYLE_FUNCTION.set(node.name.getText(), node.body);
        } else {
          INNER_STYLE_FUNCTION.set(node.name.getText(), node.body);
        }
        STYLES_ATTRIBUTE.add(node.name.getText());
        BUILDIN_STYLE_NAMES.add(node.name.getText());
      }
    }
    if (hasDecorator(node, COMPONENT_CONCURRENT_DECORATOR)) {
      // ark compiler's feature
      checkConcurrentDecorator(node, log, sourceFileNode);
    }
  }
  node.getChildren().forEach((item: ts.Node) => visitAllNode(item, sourceFileNode, allComponentNames, log));
}

export function checkAllNode(
  node: ts.EtsComponentExpression,
  allComponentNames: Set<string>,
  sourceFileNode: ts.SourceFile,
  log: LogInfo[]
): void {
  if (ts.isIdentifier(node.expression)) {
    checkNoChildComponent(node, sourceFileNode, log);
    checkOneChildComponent(node, allComponentNames, sourceFileNode, log);
    checkSpecificChildComponent(node, allComponentNames, sourceFileNode, log);
  }
}

interface ParamType {
  name: string,
  value: string,
}

function checkNoChildComponent(node: ts.EtsComponentExpression, sourceFileNode: ts.SourceFile, log: LogInfo[]): void {
  const isCheckType: ParamType = { name: null, value: null};
  if (hasChild(node, isCheckType)) {
    const componentName: string = (node.expression as ts.Identifier).escapedText.toString();
    const pos: number = node.expression.getStart();
    const message: string = isCheckType.name === null ?
      `The component '${componentName}' can't have any child.` :
      `When the component '${componentName}' set '${isCheckType.name}' is '${isCheckType.value}'` +
        `, can't have any child.`;
    addLog(LogType.ERROR, message, pos, log, sourceFileNode);
  }
}

function hasChild(node: ts.EtsComponentExpression, isCheckType: ParamType): boolean {
  const nodeName: ts.Identifier = node.expression as ts.Identifier;
  if ((AUTOMIC_COMPONENT.has(nodeName.escapedText.toString()) || judgeComponentType(nodeName, node, isCheckType)) &&
    getNextNode(node)) {
    return true;
  }
  return false;
}

function judgeComponentType(nodeName: ts.Identifier, etsComponentExpression: ts.EtsComponentExpression,
  isCheckType: ParamType): boolean {
  return COMPONENT_TOGGLE === nodeName.escapedText.toString() &&
    etsComponentExpression.arguments && etsComponentExpression.arguments[0] &&
    ts.isObjectLiteralExpression(etsComponentExpression.arguments[0]) &&
    etsComponentExpression.arguments[0].getText() &&
    judgeToggleComponentParamType(etsComponentExpression.arguments[0].getText(), isCheckType);
}

function judgeToggleComponentParamType(param: string, isCheckType: ParamType): boolean {
  if (param.indexOf(RESOURCE_NAME_TYPE) > -1) {
    isCheckType.name = RESOURCE_NAME_TYPE;
    const match: string[] = param.match(/\b(Checkbox|Switch|Button)\b/);
    if (match && match.length) {
      isCheckType.value = match[0];
      if (isCheckType.value === COMPONENT_BUTTON) {
        return false;
      }
      return true;
    }
  }
  return false;
}

function getNextNode(node: ts.EtsComponentExpression): ts.Block {
  if (node.body && ts.isBlock(node.body)) {
    const statementsArray: ts.Block = node.body;
    return statementsArray;
  }
}

function checkOneChildComponent(node: ts.EtsComponentExpression, allComponentNames: Set<string>,
  sourceFileNode: ts.SourceFile, log: LogInfo[]): void {
  const isCheckType: ParamType = { name: null, value: null};
  if (hasNonSingleChild(node, allComponentNames, isCheckType)) {
    const componentName: string = (node.expression as ts.Identifier).escapedText.toString();
    const pos: number = node.expression.getStart();
    const message: string = isCheckType.name === null ?
      `The component '${componentName}' can only have a single child component.` :
      `When the component '${componentName}' set '${isCheckType.name}' is ` +
        `'${isCheckType.value}', can only have a single child component.`;
    addLog(LogType.ERROR, message, pos, log, sourceFileNode);
  }
}

function hasNonSingleChild(node: ts.EtsComponentExpression, allComponentNames: Set<string>,
  isCheckType: ParamType): boolean {
  const nodeName: ts.Identifier = node.expression as ts.Identifier;
  const BlockNode: ts.Block = getNextNode(node);
  if (SINGLE_CHILD_COMPONENT.has(nodeName.escapedText.toString()) || !judgeComponentType(nodeName, node, isCheckType)
    && isCheckType.value === COMPONENT_BUTTON) {
    if (!BlockNode) {
      return false;
    }
    if (BlockNode && BlockNode.statements) {
      const length: number = BlockNode.statements.length;
      if (!length) {
        return false;
      }
      if (length > 3) {
        return true;
      }
      const childCount: number = getBlockChildrenCount(BlockNode, allComponentNames);
      if (childCount > 1) {
        return true;
      }
    }
  }
  return false;
}

function getBlockChildrenCount(blockNode: ts.Block, allComponentNames: Set<string>): number {
  let maxCount: number = 0;
  const length: number = blockNode.statements.length;
  for (let i = 0; i < length; ++i) {
    const item: ts.Node = blockNode.statements[i];
    if (ts.isExpressionStatement(item) && ts.isCallExpression(item.expression) &&
      isForEachComponent(item.expression)) {
      maxCount += 2;
    }
    if (ts.isIfStatement(item)) {
      maxCount += getIfChildrenCount(item, allComponentNames);
    }
    if (ts.isExpressionStatement(item) && ts.isEtsComponentExpression(item.expression)) {
      maxCount += 1;
    }
    if (ts.isExpressionStatement(item) && ts.isCallExpression(item.expression)) {
      let newNode: any = item.expression;
      while (newNode.expression) {
        if (ts.isEtsComponentExpression(newNode) || ts.isCallExpression(newNode) &&
          isComponent(newNode, allComponentNames)) {
          maxCount += 1;
        }
        newNode = newNode.expression;
      }
    }
    if (maxCount > 1) {
      break;
    }
  }
  return maxCount;
}

function isComponent(node: ts.EtsComponentExpression | ts.CallExpression, allComponentNames: Set<string>): boolean {
  if (ts.isIdentifier(node.expression) &&
    allComponentNames.has(node.expression.escapedText.toString())) {
    return true;
  }
  return false;
}

function isForEachComponent(node: ts.EtsComponentExpression | ts.CallExpression): boolean {
  if (ts.isIdentifier(node.expression)) {
    const componentName: string = node.expression.escapedText.toString();
    return componentName === COMPONENT_FOREACH || componentName === COMPONENT_LAZYFOREACH;
  }
  return false;
}

function getIfChildrenCount(ifNode: ts.IfStatement, allComponentNames: Set<string>): number {
  const maxCount: number =
    Math.max(getStatementCount(ifNode.thenStatement, allComponentNames),
      getStatementCount(ifNode.elseStatement, allComponentNames));
  return maxCount;
}

function getStatementCount(node: ts.Node, allComponentNames: Set<string>): number {
  let maxCount: number = 0;
  if (!node) {
    return maxCount;
  } else if (ts.isBlock(node)) {
    maxCount = getBlockChildrenCount(node, allComponentNames);
  } else if (ts.isIfStatement(node)) {
    maxCount = getIfChildrenCount(node, allComponentNames);
  } else if (ts.isExpressionStatement(node) && ts.isEtsComponentExpression(node.expression) &&
    isForEachComponent(node.expression)) {
    maxCount = 2;
  } else if (ts.isExpressionStatement(node) && ts.isEtsComponentExpression(node.expression) &&
    !isForEachComponent(node.expression) && isComponent(node.expression, allComponentNames)) {
    maxCount = 1;
  }
  return maxCount;
}

function checkSpecificChildComponent(node: ts.EtsComponentExpression, allComponentNames: Set<string>,
  sourceFileNode: ts.SourceFile, log: LogInfo[]): void {
  if (hasNonspecificChild(node, allComponentNames)) {
    const componentName: string = (node.expression as ts.Identifier).escapedText.toString();
    const pos: number = node.expression.getStart();
    const specificChildArray: string =
      Array.from(SPECIFIC_CHILD_COMPONENT.get(componentName)).join(' and ');
    const message: string =
      `The component '${componentName}' can only have the child component ${specificChildArray}.`;
    addLog(LogType.ERROR, message, pos, log, sourceFileNode);
  }
}

function hasNonspecificChild(node: ts.EtsComponentExpression,
  allComponentNames: Set<string>): boolean {
  const nodeName: ts.Identifier = node.expression as ts.Identifier;
  const nodeNameString: string = nodeName.escapedText.toString();
  const blockNode: ts.Block = getNextNode(node);
  let isNonspecific: boolean = false;
  if (SPECIFIC_CHILD_COMPONENT.has(nodeNameString) && blockNode) {
    const specificChildSet: Set<string> = SPECIFIC_CHILD_COMPONENT.get(nodeNameString);
    isNonspecific = isNonspecificChildBlock(blockNode, specificChildSet, allComponentNames);
    if (isNonspecific) {
      return isNonspecific;
    }
  }
  return isNonspecific;
}

function isNonspecificChildBlock(blockNode: ts.Block, specificChildSet: Set<string>,
  allComponentNames: Set<string>): boolean {
  if (blockNode.statements) {
    const length: number = blockNode.statements.length;
    for (let i = 0; i < length; ++i) {
      const item: ts.Node = blockNode.statements[i];
      if (ts.isIfStatement(item) && isNonspecificChildIf(item, specificChildSet, allComponentNames)) {
        return true;
      }
      if (ts.isExpressionStatement(item) && ts.isCallExpression(item.expression) &&
        isForEachComponent(item.expression) &&
        isNonspecificChildForEach(item.expression, specificChildSet, allComponentNames)) {
        return true;
      }
      if (ts.isBlock(item) && isNonspecificChildBlock(item, specificChildSet, allComponentNames)) {
        return true;
      }
      if (ts.isExpressionStatement(item)) {
        let newNode: any = item.expression;
        while (newNode.expression) {
          if (ts.isEtsComponentExpression(newNode) && ts.isIdentifier(newNode.expression) &&
          !isForEachComponent(newNode) && isComponent(newNode, allComponentNames)) {
            const isNonspecific: boolean =
            isNonspecificChildNonForEach(newNode, specificChildSet);
            if (isNonspecific) {
              return isNonspecific;
            }
            if (i + 1 < length && ts.isBlock(blockNode.statements[i + 1])) {
              ++i;
            }
          }
          newNode = newNode.expression;
        }
      }
    }
  }
  return false;
}

function isNonspecificChildIf(node: ts.IfStatement, specificChildSet: Set<string>,
  allComponentNames: Set<string>): boolean {
  return isNonspecificChildIfStatement(node.thenStatement, specificChildSet, allComponentNames) ||
    isNonspecificChildIfStatement(node.elseStatement, specificChildSet, allComponentNames);
}

function isNonspecificChildForEach(node: ts.EtsComponentExpression, specificChildSet: Set<string>,
  allComponentNames: Set<string>): boolean {
  if (ts.isCallExpression(node) && node.arguments &&
    node.arguments.length > 1 && ts.isArrowFunction(node.arguments[1])) {
    const arrowFunction: ts.ArrowFunction = node.arguments[1] as ts.ArrowFunction;
    const body: ts.Block | ts.EtsComponentExpression | ts.IfStatement =
      arrowFunction.body as ts.Block | ts.EtsComponentExpression | ts.IfStatement;
    if (!body) {
      return false;
    }
    if (ts.isBlock(body) && isNonspecificChildBlock(body, specificChildSet, allComponentNames)) {
      return true;
    }
    if (ts.isIfStatement(body) && isNonspecificChildIf(body, specificChildSet, allComponentNames)) {
      return true;
    }
    if (ts.isCallExpression(body) && isForEachComponent(body) &&
      isNonspecificChildForEach(body, specificChildSet, allComponentNames)) {
      return true;
    }
    if (ts.isEtsComponentExpression(body) && !isForEachComponent(body) &&
      isComponent(body, allComponentNames) &&
      isNonspecificChildNonForEach(body, specificChildSet)) {
      return true;
    }
  }
  return false;
}

function isNonspecificChildNonForEach(node: ts.EtsComponentExpression,
  specificChildSet: Set<string>): boolean {
  if (ts.isIdentifier(node.expression) &&
    !specificChildSet.has(node.expression.escapedText.toString())) {
    return true;
  }
  return false;
}

function isNonspecificChildIfStatement(node: ts.Node, specificChildSet: Set<string>,
  allComponentNames: Set<string>): boolean {
  if (!node) {
    return false;
  }
  if (ts.isBlock(node) && isNonspecificChildBlock(node, specificChildSet, allComponentNames)) {
    return true;
  }
  if (ts.isIfStatement(node) && isNonspecificChildIf(node, specificChildSet, allComponentNames)) {
    return true;
  }
  if (ts.isExpressionStatement(node) && ts.isEtsComponentExpression(node.expression) &&
    isForEachComponent(node.expression) &&
    isNonspecificChildForEach(node.expression, specificChildSet, allComponentNames)) {
    return true;
  }
  if (ts.isExpressionStatement(node) && ts.isEtsComponentExpression(node.expression) &&
    !isForEachComponent(node.expression) && isComponent(node.expression, allComponentNames) &&
    isNonspecificChildNonForEach(node.expression, specificChildSet)) {
    return true;
  }
  return false;
}

function collectComponentProps(node: ts.StructDeclaration): void {
  const componentName: string = node.name.getText();
  const ComponentSet: IComponentSet = getComponentSet(node);
  propertyCollection.set(componentName, ComponentSet.properties);
  stateCollection.set(componentName, ComponentSet.states);
  linkCollection.set(componentName, ComponentSet.links);
  propCollection.set(componentName, ComponentSet.props);
  regularCollection.set(componentName, ComponentSet.regulars);
  storagePropCollection.set(componentName, ComponentSet.storageProps);
  storageLinkCollection.set(componentName, ComponentSet.storageLinks);
  provideCollection.set(componentName, ComponentSet.provides);
  consumeCollection.set(componentName, ComponentSet.consumes);
  objectLinkCollection.set(componentName, ComponentSet.objectLinks);
  localStorageLinkCollection.set(componentName, ComponentSet.localStorageLink);
  localStoragePropCollection.set(componentName, ComponentSet.localStorageProp);
  builderParamObjectCollection.set(componentName, ComponentSet.builderParams);
  builderParamInitialization.set(componentName, ComponentSet.builderParamData);
}

export function getComponentSet(node: ts.StructDeclaration): IComponentSet {
  const properties: Set<string> = new Set();
  const states: Set<string> = new Set();
  const links: Set<string> = new Set();
  const props: Set<string> = new Set();
  const regulars: Set<string> = new Set();
  const storageProps: Set<string> = new Set();
  const storageLinks: Set<string> = new Set();
  const provides: Set<string> = new Set();
  const consumes: Set<string> = new Set();
  const objectLinks: Set<string> = new Set();
  const builderParams: Set<string> = new Set();
  const localStorageLink: Map<string, Set<string>> = new Map();
  const localStorageProp: Map<string, Set<string>> = new Map();
  const builderParamData: Set<string> = new Set();
  traversalComponentProps(node, properties, regulars, states, links, props, storageProps,
    storageLinks, provides, consumes, objectLinks, localStorageLink, localStorageProp, builderParams,
    builderParamData);
  return {
    properties, regulars, states, links, props, storageProps, storageLinks, provides,
    consumes, objectLinks, localStorageLink, localStorageProp, builderParams, builderParamData
  };
}

function traversalComponentProps(node: ts.StructDeclaration, properties: Set<string>,
  regulars: Set<string>, states: Set<string>, links: Set<string>, props: Set<string>,
  storageProps: Set<string>, storageLinks: Set<string>, provides: Set<string>,
  consumes: Set<string>, objectLinks: Set<string>,
  localStorageLink: Map<string, Set<string>>, localStorageProp: Map<string, Set<string>>,
  builderParams: Set<string>, builderParamData: Set<string>): void {
  let isStatic: boolean = true;
  if (node.members) {
    const currentMethodCollection: Set<string> = new Set();
    node.members.forEach(item => {
      if (ts.isPropertyDeclaration(item) && ts.isIdentifier(item.name)) {
        const propertyName: string = item.name.getText();
        properties.add(propertyName);
        if (!item.decorators || !item.decorators.length) {
          regulars.add(propertyName);
        } else {
          isStatic = false;
          for (let i = 0; i < item.decorators.length; i++) {
            const decoratorName: string = item.decorators[i].getText().replace(/\(.*\)$/, '').trim();
            if (INNER_COMPONENT_MEMBER_DECORATORS.has(decoratorName)) {
              dollarCollection.add('$' + propertyName);
              collectionStates(item.decorators[i], decoratorName, propertyName, states, links, props, storageProps,
                storageLinks, provides, consumes, objectLinks, localStorageLink, localStorageProp,
                builderParams, item.initializer, builderParamData);
            }
          }
        }
      }
      if (ts.isMethodDeclaration(item) && item.name && ts.isIdentifier(item.name)) {
        validateStateVariable(item);
        currentMethodCollection.add(item.name.getText());
      }
    });
    classMethodCollection.set(node.name.getText(), currentMethodCollection);
  }
  isStaticViewCollection.set(node.name.getText(), isStatic);
}

function collectionStates(node: ts.Decorator, decorator: string, name: string,
  states: Set<string>, links: Set<string>, props: Set<string>, storageProps: Set<string>,
  storageLinks: Set<string>, provides: Set<string>, consumes: Set<string>, objectLinks: Set<string>,
  localStorageLink: Map<string, Set<string>>, localStorageProp: Map<string, Set<string>>,
  builderParams: Set<string>, initializationtName: ts.Expression, builderParamData: Set<string>): void {
  switch (decorator) {
    case COMPONENT_STATE_DECORATOR:
      states.add(name);
      break;
    case COMPONENT_LINK_DECORATOR:
      links.add(name);
      break;
    case COMPONENT_PROP_DECORATOR:
      props.add(name);
      break;
    case COMPONENT_STORAGE_PROP_DECORATOR:
      storageProps.add(name);
      break;
    case COMPONENT_STORAGE_LINK_DECORATOR:
      storageLinks.add(name);
      break;
    case COMPONENT_PROVIDE_DECORATOR:
      provides.add(name);
      break;
    case COMPONENT_CONSUME_DECORATOR:
      consumes.add(name);
      break;
    case COMPONENT_OBJECT_LINK_DECORATOR:
      objectLinks.add(name);
      break;
    case COMPONENT_BUILDERPARAM_DECORATOR:
      if (initializationtName) {
        builderParamData.add(name);
      }
      builderParams.add(name);
      break;
    case COMPONENT_LOCAL_STORAGE_LINK_DECORATOR :
      collectionlocalStorageParam(node, name, localStorageLink);
      break;
    case COMPONENT_LOCAL_STORAGE_PROP_DECORATOR:
      collectionlocalStorageParam(node, name, localStorageProp);
      break;
  }
}

function collectionlocalStorageParam(node: ts.Decorator, name: string,
  localStorage: Map<string, Set<string>>): void {
  const localStorageParam: Set<string> = new Set();
  if (node && ts.isCallExpression(node.expression) && node.expression.arguments &&
    node.expression.arguments.length && ts.isStringLiteral(node.expression.arguments[0])) {
    localStorage.set(name, localStorageParam.add(
      node.expression.arguments[0].getText().replace(/\"|'/g, '')));
  }
}

export interface ReplaceResult {
  content: string,
  log: LogInfo[]
}

export function sourceReplace(source: string, sourcePath: string): ReplaceResult {
  let content: string = source;
  const log: LogInfo[] = [];
  content = preprocessExtend(content);
  content = preprocessNewExtend(content);
  // process @system.
  content = processSystemApi(content, false, sourcePath);
  CollectImportNames(content, sourcePath);

  return {
    content: content,
    log: log
  };
}

export function preprocessExtend(content: string, extendCollection?: Set<string>): string {
  const REG_EXTEND: RegExp = /@Extend(\s+)([^\.\s]+)\.([^\(]+)\(/gm;
  return content.replace(REG_EXTEND, (item, item1, item2, item3) => {
    collectExtend(EXTEND_ATTRIBUTE, item2, '__' + item2 + '__' + item3);
    collectExtend(EXTEND_ATTRIBUTE, item2, item3);
    if (extendCollection) {
      extendCollection.add(item3);
    }
    return `@Extend(${item2})${item1}function __${item2}__${item3}(`;
  });
}

export function preprocessNewExtend(content: string, extendCollection?: Set<string>): string {
  const REG_EXTEND: RegExp = /@Extend\s*\([^\)]+\)\s*function\s+([^\(\s]+)\s*\(/gm;
  return content.replace(REG_EXTEND, (item, item1) => {
    if (extendCollection) {
      extendCollection.add(item1);
    }
    return item;
  });
}

function replaceSystemApi(item: string, systemValue: string, moduleType: string, systemKey: string): string {
  // if change format, please update regexp in transformModuleSpecifier
  if (NATIVE_MODULE.has(`${moduleType}.${systemKey}`)) {
    item = `var ${systemValue} = globalThis.requireNativeModule('${moduleType}.${systemKey}')`;
  } else if (moduleType === SYSTEM_PLUGIN || moduleType === OHOS_PLUGIN) {
    item = `var ${systemValue} = globalThis.requireNapi('${systemKey}')`;
  }
  return item;
}

function replaceLibSo(importValue: string, libSoKey: string, sourcePath: string = null): string {
  if (sourcePath) {
    useOSFiles.add(sourcePath);
  }
  // if change format, please update regexp in transformModuleSpecifier
  return projectConfig.bundleName && projectConfig.moduleName
    ? `var ${importValue} = globalThis.requireNapi("${libSoKey}", true, "${projectConfig.bundleName}/${projectConfig.moduleName}");`
    : `var ${importValue} = globalThis.requireNapi("${libSoKey}", true);`;
}

function replaceOhmStartsWithBundle(url: string, item: string, importValue: string, moduleRequest: string, sourcePath: string): string {
  const urlResult: RegExpMatchArray | null = url.match(/^(\S+)\/(\S+)\/(\S+)\/(\S+)$/);
  if (urlResult) {
    const moduleKind: string = urlResult[3];
    if (moduleKind === 'lib') {
      const libSoKey: string = urlResult[4];
      item = replaceLibSo(importValue, libSoKey, sourcePath);
    }
  }
  return item;
}

function replaceOhmStartsWithModule(url: string, item: string, importValue: string, moduleRequest: string, sourcePath: string): string {
  const urlResult: RegExpMatchArray | null = url.match(/^(\S+)\/(\S+)\/(\S+)$/);
  if (urlResult && projectConfig.aceModuleJsonPath) {
    const moduleName: string = urlResult[1];
    const moduleKind: string = urlResult[2];
    const modulePath: string = urlResult[3];
    const bundleName: string = getPackageInfo(projectConfig.aceModuleJsonPath)[0];
    moduleRequest = `@bundle:${bundleName}/${moduleName}/${moduleKind}/${modulePath}`;
    item = moduleKind === 'lib' ? replaceLibSo(importValue, modulePath, sourcePath) :
      item.replace(/['"](\S+)['"]/, '\"' + moduleRequest + '\"');
  }
  return item;
}

function replaceOhmStartsWithOhos(url: string, item: string, importValue:string, moduleRequest: string, isSystemModule: boolean): string {
  url = url.replace('/', '.');
  const urlResult: RegExpMatchArray | null = url.match(/^system\.(\S+)/);
  moduleRequest = urlResult ? `@${url}` : `@ohos.${url}`;
  if (!isSystemModule) {
    item = item.replace(/['"](\S+)['"]/, '\"' + moduleRequest + '\"');
  } else {
    const moduleType: string = urlResult ? 'system' : 'ohos';
    const systemKey: string = urlResult ? url.substring(7) : url;
    item = replaceSystemApi(item, importValue, moduleType, systemKey);
  }
  return item;
}

function replaceOhmStartsWithLocal(url: string, item: string, importValue: string, moduleRequest: string, sourcePath: string): string {
  const result: RegExpMatchArray | null = sourcePath.match(/(\S+)(\/|\\)src(\/|\\)(?:main|ohosTest)(\/|\\)(ets|js)(\/|\\)(\S+)/);
  if (result && projectConfig.aceModuleJsonPath) {
    const packageInfo: string[] = getPackageInfo(projectConfig.aceModuleJsonPath);
    const urlResult: RegExpMatchArray | null = url.match(/^\/(ets|js|lib|node_modules)\/(\S+)$/);
    if (urlResult) {
      const moduleKind: string = urlResult[1];
      const modulePath: string = urlResult[2];
      if (moduleKind === 'lib') {
        item = replaceLibSo(importValue, modulePath, sourcePath);
      } else if (moduleKind === 'node_modules') {
        moduleRequest = `${modulePath}`;
        item = item.replace(/['"](\S+)['"]/, '\"' + moduleRequest + '\"');
      } else {
        moduleRequest = `@bundle:${packageInfo[0]}/${packageInfo[1]}/${moduleKind}/${modulePath}`;
        item = item.replace(/['"](\S+)['"]/, '\"' + moduleRequest + '\"');
      }
    }
  }
  return item;
}

function replaceOhmUrl(isSystemModule: boolean, item: string, importValue: string, moduleRequest: string, sourcePath: string = null): string {
  const result: RegExpMatchArray = moduleRequest.match(/^@(\S+):(\S+)$/);
  const urlType: string = result[1];
  const url: string = result[2];
  switch (urlType) {
    case 'bundle': {
      item = replaceOhmStartsWithBundle(url, item, importValue, moduleRequest, sourcePath);
      break;
    }
    case 'module': {
      item = replaceOhmStartsWithModule(url, item, importValue, moduleRequest, sourcePath);
      break;
    }
    case 'ohos': {
      item = replaceOhmStartsWithOhos(url, item, importValue, moduleRequest, isSystemModule);
      break;
    }
    case 'lib': {
      item = replaceLibSo(importValue, url, sourcePath);
      break;
    }
    case 'local': {
      item = replaceOhmStartsWithLocal(url, item, importValue, moduleRequest, sourcePath);
      break;
    }
    default:
      logger.error('\u001b[31m', `ArkTS:ERROR Incorrect OpenHarmony module kind: ${urlType}`, '\u001b[39m');
  }
  return item;
}

export function processSystemApi(content: string, isProcessAllowList: boolean = false,
  sourcePath: string = null, isSystemModule: boolean = false): string {
  if (isProcessAllowList && projectConfig.compileMode === ESMODULE) {
    // remove the unused system api import decl like following when compile as [esmodule]
    // in the result_process phase
    // e.g. import "@ohos.application.xxx"
    const REG_UNUSED_SYSTEM_IMPORT: RegExp = /import(?:\s*)['"]@(system|ohos)\.(\S+)['"]/g;
    content = content.replace(REG_UNUSED_SYSTEM_IMPORT, '');
  }

  const REG_IMPORT_DECL: RegExp = isProcessAllowList ? projectConfig.compileMode === ESMODULE ?
    /import\s+(.+)\s+from\s+['"]@(system|ohos)\.(\S+)['"]/g :
    /(import|const)\s+(.+)\s*=\s*(\_\_importDefault\()?require\(\s*['"]@(system|ohos)\.(\S+)['"]\s*\)(\))?/g :
    /(import|export)\s+(?:(.+)|\{([\s\S]+)\})\s+from\s+['"](\S+)['"]|import\s+(.+)\s*=\s*require\(\s*['"](\S+)['"]\s*\)/g;

  const systemValueCollection: Set<string> = new Set();
  const processedContent: string = content.replace(REG_IMPORT_DECL, (item, item1, item2, item3, item4, item5, item6) => {
    const importValue: string = isProcessAllowList ? projectConfig.compileMode === ESMODULE ? item1 : item2 : item2 || item5;

    if (isProcessAllowList) {
      systemValueCollection.add(importValue);
      if (projectConfig.compileMode !== ESMODULE) {
        collectSourcemapNames(sourcePath, importValue, item5);
        return replaceSystemApi(item, importValue, item4, item5);
      }
      collectSourcemapNames(sourcePath, importValue, item3);
      return replaceSystemApi(item, importValue, item2, item3);
    }

    const moduleRequest: string = item4 || item6;
    if (/^@(system|ohos)\./.test(moduleRequest)) { // ohos/system.api
      // ets & ts file need compile with .d.ts, so do not replace at the phase of pre_process
      if (!isSystemModule) {
        return item;
      }
      const result: RegExpMatchArray = moduleRequest.match(/^@(system|ohos)\.(\S+)$/);
      const moduleType: string = result[1];
      const apiName: string = result[2];
      return replaceSystemApi(item, importValue, moduleType, apiName);
    } else if (/^lib(\S+)\.so$/.test(moduleRequest)) { // libxxx.so
      const result: RegExpMatchArray = moduleRequest.match(/^lib(\S+)\.so$/);
      const libSoKey: string = result[1];
      return replaceLibSo(importValue, libSoKey, sourcePath);
    }
    return item;
  });
  return processInnerModule(processedContent, systemValueCollection);
}

function collectSourcemapNames(sourcePath: string, changedName: string, originalName: string): void {
  if (sourcePath == null) {
    return;
  }
  const cleanSourcePath: string = sourcePath.replace('.ets', '.js').replace('.ts', '.js');
  if (!sourcemapNamesCollection.has(cleanSourcePath)) {
    return;
  }

  let map: Map<string, string> = sourcemapNamesCollection.get(cleanSourcePath);
  if (map.has(changedName)) {
    return;
  }

  for (let entry of originalImportNamesMap.entries()) {
    const key: string = entry[0];
    const value: string = entry[1];
    if (value === '@ohos.' + originalName || value === '@system.' + originalName) {
      map.set(changedName.trim(), key);
      sourcemapNamesCollection.set(cleanSourcePath, map);
      originalImportNamesMap.delete(key);
      break;
    }
  }
}

export function CollectImportNames(content: string, sourcePath: string = null): void {
  const REG_IMPORT_DECL: RegExp =
    /(import|export)\s+(.+)\s+from\s+['"](\S+)['"]|import\s+(.+)\s*=\s*require\(\s*['"](\S+)['"]\s*\)/g;

  const decls: string[] = content.match(REG_IMPORT_DECL);
  if (decls != undefined) {
    decls.forEach(decl => {
      const parts: string[] = decl.split(' ');
      if (parts.length === 4 && parts[0] === 'import' && parts[2] === 'from' && !parts[3].includes('.so')) {
        originalImportNamesMap.set(parts[1], parts[3].replace(/'/g, ''));
      }
    })
  }

  if (sourcePath && sourcePath != null) {
    const cleanSourcePath: string = sourcePath.replace('.ets', '.js').replace('.ts', '.js');
    if (!sourcemapNamesCollection.has(cleanSourcePath)) {
      sourcemapNamesCollection.set(cleanSourcePath, new Map());
    }
  }
}

function processInnerModule(content: string, systemValueCollection: Set<string>): string {
  systemValueCollection.forEach(element => {
    const target: string = element.trim() + '.default';
    while (content.includes(target)) {
      content = content.replace(target, element.trim());
    }
  });
  return content;
}

const VALIDATE_MODULE_REG: RegExp = new RegExp('^(' + VALIDATE_MODULE.join('|') + ')');
function validateAllowListModule(moduleType: string, systemKey: string): boolean {
  return moduleType === 'ohos' && VALIDATE_MODULE_REG.test(systemKey);
}

export function resetComponentCollection() {
  componentCollection.entryComponent = null;
  componentCollection.entryComponentPos = null;
  componentCollection.previewComponent = new Array();
}

function checkEntryComponent(node: ts.StructDeclaration, log: LogInfo[], sourceFile: ts.SourceFile): void {
  if (node.modifiers) {
    for (let i = 0; i < node.modifiers.length; i++) {
      if (node.modifiers[i].kind === ts.SyntaxKind.ExportKeyword) {
        const message: string = `It's not a recommended way to export struct with @Entry decorator, ` +
          `which may cause ACE Engine error in component preview mode.`;
        addLog(LogType.WARN, message, node.getStart(), log, sourceFile);
        break;
      }
    }
  }
}

function validateStateVariable(node: ts.MethodDeclaration): void {
  if (node.decorators && node.decorators.length) {
    for (let i = 0; i < node.decorators.length; i++) {
      const decoratorName: string = node.decorators[i].getText().replace(/\(.*\)$/,'').trim();
      if (CARD_ENABLE_DECORATORS[decoratorName]) {
        validatorCard(transformLog.errors, CARD_LOG_TYPE_DECORATORS,
          node.decorators[i].getStart(), decoratorName);
      }
      if (INNER_COMPONENT_MEMBER_DECORATORS.has(decoratorName)) {
        transformLog.errors.push({
          type: LogType.ERROR,
          message: `'${node.decorators[i].getText()}' can not decorate the method.`,
          pos: node.decorators[i].getStart()
        });
      }
    }
  }
}

export function getObservedPropertyCollection(className: string): Set<string> {
  const observedProperthCollection: Set<string> = new Set([
    ...stateCollection.get(className),
    ...linkCollection.get(className),
    ...propCollection.get(className),
    ...storageLinkCollection.get(className),
    ...storageLinkCollection.get(className),
    ...provideCollection.get(className),
    ...consumeCollection.get(className),
    ...objectLinkCollection.get(className)
  ]);
  getLocalStorageCollection(className, observedProperthCollection);
  return observedProperthCollection;
}

export function getLocalStorageCollection(componentName: string, collection: Set<string>): void {
  if (localStorageLinkCollection.get(componentName)) {
    for (const key of localStorageLinkCollection.get(componentName).keys()) {
      collection.add(key);
    }
  }
  if (localStoragePropCollection.get(componentName)) {
    for (const key of localStoragePropCollection.get(componentName).keys()) {
      collection.add(key);
    }
  }
}
