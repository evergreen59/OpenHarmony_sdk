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


declare enum FontStyle {
  Normal,
  Italic
}

declare enum FontWeight {
  Lighter,
  Normal,
  Regular,
  Medium,
  Bold,
  Bolder
}

declare enum TextAlign {
  Center,
  Start,
  End
}

declare enum TextOverflow {
  Clip,
  Ellipsis,
  None
}

declare enum TextDecorationType {
  None,
  Underline,
  Overline,
  LineThrough
}

declare enum TextCase {
  Normal,
  LowerCase,
  UpperCase
}

interface TextInterface {
  (content?: string): TextAttribute;
}

declare class TextAttribute extends CommonMethod<TextAttribute> {
  fontColor(value: number | string): TextAttribute;

  fontSize(value: number | string): TextAttribute;

  minFontSize(value: number | string): TextAttribute;

  maxFontSize(value: number | string): TextAttribute;

  fontStyle(value: FontStyle): TextAttribute;

  fontWeight(value: number | FontWeight | string): TextAttribute;

  textAlign(value: TextAlign): TextAttribute;

  lineHeight(value: number | string): TextAttribute;

  textOverflow(value: { overflow: TextOverflow }): TextAttribute;

  fontFamily(value: string): TextAttribute;

  maxLines(value: number): TextAttribute;

  decoration(value: { type: TextDecorationType, color?: number | string }): TextAttribute;

  letterSpacing(value: number | string): TextAttribute;

  textCase(value: TextCase): TextAttribute;

  baselineOffset(value: number | string): TextAttribute;
}

declare const  Text: TextInterface;

declare const TextInstance: TextAttribute;