/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "components/ui_edit_text.h"

#include <codecvt>
#include <locale>

#include "font/ui_font.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"
#include "themes/theme_manager.h"

namespace OHOS {
namespace {
constexpr char16_t PASSWORD_DOT = u'•'; // dot for password type
constexpr uint16_t DEFAULT_TEXT_OFFSET = 5;
constexpr uint16_t DEFAULT_CURSOR_OFFSET = 2;
constexpr uint16_t DEFAULT_CURSOR_WIDTH = 2;
} // namespace name

class CursorAnimator : public Animator, public AnimatorCallback {
public:
    explicit CursorAnimator(UIEditText* view) : Animator(this, view, 0, true), editText_(view) {}

    virtual ~CursorAnimator() {}

    void Callback(UIView* view) override
    {
        if ((view == nullptr) || (editText_ == nullptr)) {
            return;
        }

        uint32_t curTime = GetRunTime();
        if (curTime == preTime_) {
            return;
        }
        uint32_t duration = (curTime > preTime_) ? (curTime - preTime_) : (UINT32_MAX - preTime_ + curTime);
        if (duration < CURSOR_ANIMATOT_DURATION) {
            return;
        }
        preTime_ = curTime;
        editText_->drawCursor_ = !editText_->drawCursor_;
        view->Invalidate();
    }

    void StartAnimator()
    {
        if (editText_ == nullptr) {
            return;
        }
        Start();
        preTime_ = GetRunTime();
        editText_->drawCursor_ = false;
    }

    void StopAnimator()
    {
        if (editText_ == nullptr) {
            return;
        }
        Stop();
        editText_->drawCursor_ = false;
        editText_->Invalidate();
    }

private:
    uint32_t preTime_ = 0;
    UIEditText* editText_ = nullptr;
    static constexpr uint16_t CURSOR_ANIMATOT_DURATION = 650;
};

UIEditText::UIEditText()
    : inputText_(nullptr),
      placeholderText_(nullptr),
      needRefresh_(false),
      useTextColor_(false),
      isFocused_(false),
      drawCursor_(false),
      maxLength_(MAX_TEXT_LENGTH),
      placeholderEllipsisIndex_(Text::TEXT_ELLIPSIS_END_INV),
      offsetX_(DEFAULT_TEXT_OFFSET),
      textColor_(Color::White()),
      placeholderColor_(Color::Gray()),
      cursorColor_(Color::White()),
      onChangeListener_(nullptr),
      cursorAnimator_(nullptr)
{
    touchable_ = true;
    focusable_ = true;
    Theme* theme = ThemeManager::GetInstance().GetCurrent();
    Style& style = (theme != nullptr) ? (theme->GetEditTextStyle()) : (StyleDefault::GetEditTextStyle());
    UIView::SetStyle(style);
}

UIEditText::~UIEditText()
{
    if (cursorAnimator_ != nullptr) {
        delete cursorAnimator_;
        cursorAnimator_ = nullptr;
    }
    if (inputText_ != nullptr) {
        delete inputText_;
        inputText_ = nullptr;
    }
    if (placeholderText_ != nullptr) {
        delete placeholderText_;
        placeholderText_ = nullptr;
    }
}

bool UIEditText::OnPressEvent(const PressEvent& event)
{
    if (touchable_) {
        RequestFocus();
    }
    return UIView::OnPressEvent(event);
}

void UIEditText::Focus()
{
    if (focusable_) {
        if (cursorAnimator_ == nullptr) {
            cursorAnimator_ = new CursorAnimator(this);
        }
        static_cast<CursorAnimator*>(cursorAnimator_)->StartAnimator();
        isFocused_ = true;
    }
    UpdateOffsetX();
    Invalidate();
    UIView::Focus();
}

void UIEditText::Blur()
{
    if (cursorAnimator_ != nullptr) {
        static_cast<CursorAnimator*>(cursorAnimator_)->StopAnimator();
    }
    isFocused_ = false;
    UpdateOffsetX();
    Invalidate();
    UIView::Blur();
}

void UIEditText::InitText()
{
    if (inputText_ == nullptr) {
        inputText_ = new Text();
        inputText_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        inputText_->SetExpandWidth(true);
        inputText_->SetExpandHeight(false);
    }

    if (placeholderText_ == nullptr) {
        placeholderText_ = new Text();
        placeholderText_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        placeholderText_->SetExpandWidth(false);
        placeholderText_->SetExpandHeight(false);
    }
}

void UIEditText::SetStyle(Style& style)
{
    UIView::SetStyle(style);
    RefreshText();
}

void UIEditText::SetStyle(uint8_t key, int64_t value)
{
    UIView::SetStyle(key, value);
    RefreshText();
}

void UIEditText::SetText(const char* text)
{
    InitText();
    if (text == nullptr) {
        return;
    }
    SetText(std::string(text));
}

void UIEditText::SetText(std::string text)
{
    UpdateTextString(text);
    UpdateInnerText();
}

const char* UIEditText::GetText()
{
    return textStr_.c_str();
}

void UIEditText::UpdateTextString(std::string text)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
    std::wstring wideText = convert.from_bytes(text);
    uint32_t textLen = wideText.length();
    uint16_t maxLength = GetMaxLength();
    if (textLen > maxLength) {
        textLen = maxLength;
    }
    std::wstring newWideText = wideText.substr(0, textLen);
    std::string newText = convert.to_bytes(newWideText);
    CheckValueChange(newText);
    textStr_ = newText;

    std::wstring dotString = std::wstring(newWideText.length(), PASSWORD_DOT);
    passwordStr_ = convert.to_bytes(dotString);
}

std::string UIEditText::GetInnerText()
{
    return textStr_;
}

std::string UIEditText::GetInnerPassword()
{
    return passwordStr_;
}

void UIEditText::SetPlaceholder(const char* text)
{
    InitText();
    placeholderText_->SetText(text);
    if (placeholderText_->IsNeedRefresh()) {
        RefreshText();
    }
}

const char* UIEditText::GetPlaceholder()
{
    if ((placeholderText_ == nullptr) || placeholderText_->GetText() == nullptr) {
        return "";
    } else {
        return placeholderText_->GetText();
    }
}

void UIEditText::SetFontId(uint16_t fontId)
{
    InitText();
    inputText_->SetFontId(fontId);
    placeholderText_->SetFontId(fontId);
    if (inputText_->IsNeedRefresh()) {
        RefreshText();
    }
}

void UIEditText::SetFont(const char* name, uint8_t size)
{
    InitText();
    inputText_->SetFont(name, size);
    placeholderText_->SetFont(name, size);
    if (inputText_->IsNeedRefresh()) {
        RefreshText();
    }
}

uint16_t UIEditText::GetTextWidth()
{
    InitText();
    if (inputText_->IsNeedRefresh()) {
        ReMeasure();
    }
    return inputText_->GetTextSize().x;
}

uint16_t UIEditText::GetTextHeight()
{
    InitText();
    if (inputText_->IsNeedRefresh()) {
        ReMeasure();
    }
    return inputText_->GetTextSize().y;
}

void UIEditText::RefreshText()
{
    Invalidate();
    placeholderEllipsisIndex_ = Text::TEXT_ELLIPSIS_END_INV;
    if (!needRefresh_) {
        needRefresh_ = true;
    }
}

void UIEditText::ReMeasure()
{
    if (!needRefresh_) {
        return;
    }
    needRefresh_ = false;
    InitText();
    Style style = GetStyleConst();
    style.textColor_ = GetTextColor();
    Rect contentRect = GetContentRect();
    int16_t width = contentRect.GetWidth() - DEFAULT_TEXT_OFFSET * 2; // 2: left and right space
    contentRect.SetWidth(width > 0 ? width : 0);
    inputText_->ReMeasureTextSize(contentRect, style);
    placeholderText_->ReMeasureTextSize(contentRect, style);
    placeholderEllipsisIndex_ = placeholderText_->GetEllipsisIndex(contentRect, style);
    placeholderText_->ReMeasureTextWidthInEllipsisMode(contentRect, style, placeholderEllipsisIndex_);

    UpdateOffsetX();
}

void UIEditText::UpdateOffsetX()
{
    if (isFocused_) {
        Point textSize = inputText_->GetTextSize();
        Rect contentRect = GetContentRect();
        int16_t curWidth = textSize.x + DEFAULT_TEXT_OFFSET * 2;
        if (contentRect.GetWidth() > curWidth) {
            offsetX_ = DEFAULT_TEXT_OFFSET;
        } else {
            offsetX_ = contentRect.GetWidth() - curWidth;
        }
    } else {
        offsetX_ = DEFAULT_TEXT_OFFSET;
    }
}

void UIEditText::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    InitText();
    UIView::OnDraw(gfxDstBuffer, invalidatedArea);

    bool drawPlaceholder = false;
    if (inputText_->GetText() != nullptr && strlen(inputText_->GetText()) > 0) {
        Style style = GetStyleConst();
        style.textColor_ = GetTextColor();
        OpacityType opa = GetMixOpaScale();
        inputText_->OnDraw(gfxDstBuffer, invalidatedArea, GetOrigRect(), GetContentRect(), offsetX_, style,
                           Text::TEXT_ELLIPSIS_END_INV, opa);
        drawPlaceholder = false;
    } else {
        Style style = GetStyleConst();
        style.textColor_ = GetPlaceholderColor();
        OpacityType opa = GetMixOpaScale();
        placeholderText_->OnDraw(gfxDstBuffer, invalidatedArea, GetOrigRect(), GetContentRect(), DEFAULT_TEXT_OFFSET,
                                 style, placeholderEllipsisIndex_, opa);
        drawPlaceholder = true;
    }

    DrawCursor(gfxDstBuffer, invalidatedArea, drawPlaceholder);
}

void UIEditText::DrawCursor(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, bool drawPlaceholder)
{
    if (!(isFocused_ && drawCursor_)) {
        return;
    }

    Style* cursorStyle = new Style();
    cursorStyle->SetStyle(STYLE_BACKGROUND_COLOR, cursorColor_.full);
    cursorStyle->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    Rect viewRect;
    int16_t cursorSpace = DEFAULT_CURSOR_OFFSET;
    if (drawPlaceholder) {
        viewRect.SetLeft(GetOrigRect().GetX() + cursorSpace + offsetX_);
    } else {
        int16_t width = inputText_->GetTextSize().x;
        viewRect.SetLeft(GetOrigRect().GetX() + width + cursorSpace + offsetX_);
    }
    viewRect.SetTop(GetOrigRect().GetTop() + (GetRect().GetHeight() - inputText_->GetFontSize()) / 2); // 2: harf size
    viewRect.SetHeight(inputText_->GetFontSize());
    viewRect.SetWidth(DEFAULT_CURSOR_WIDTH);

    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, viewRect, invalidatedArea, *cursorStyle, OPA_OPAQUE);
    delete cursorStyle;
}

void UIEditText::InsertText(std::string text)
{
    SetText(textStr_ + text);
    if (cursorAnimator_ != nullptr) {
        static_cast<CursorAnimator*>(cursorAnimator_)->StartAnimator();
    }
}

void UIEditText::DeleteBackward(uint32_t length)
{
    if ((length == 0) || (textStr_.length() == 0)) {
        return;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
    std::wstring wideText = convert.from_bytes(textStr_);
    uint32_t textLen = wideText.length();
    if (length > textLen) {
        textLen = 0;
    } else {
        textLen -= length;
    }
    std::wstring newWideText = std::wstring(wideText, 0, textLen);
    std::string newText = convert.to_bytes(newWideText);

    SetText(newText);
    if (cursorAnimator_ != nullptr) {
        static_cast<CursorAnimator*>(cursorAnimator_)->StartAnimator();
    }
}

void UIEditText::UpdateInnerText()
{
    InitText();
    if (inputType_ == InputType::TEXT_TYPE) {
        inputText_->SetText(GetInnerText().c_str());
    } else {
        inputText_->SetText(GetInnerPassword().c_str());
    }
    RefreshText();
}

void UIEditText::SetMaxLength(uint16_t maxLength)
{
    InitText();
    if (maxLength > MAX_TEXT_LENGTH) {
        maxLength = MAX_TEXT_LENGTH;
    }
    maxLength_ = maxLength;
    if (textStr_.length() > maxLength) {
        SetText(textStr_.substr(0, maxLength));
    }
}

uint16_t UIEditText::GetMaxLength()
{
    return maxLength_;
}

void UIEditText::SetInputType(InputType inputType)
{
    if (inputType_ == inputType) {
        return;
    }
    inputType_ = inputType;

    // update view
    UpdateInnerText();
}

void UIEditText::CheckValueChange(std::string text)
{
    if (onChangeListener_ == nullptr) {
        return;
    }

    if (textStr_.compare(text) != 0) {
        onChangeListener_->OnChange(*this, text.c_str());
    }
}
} // namespace OHOS
