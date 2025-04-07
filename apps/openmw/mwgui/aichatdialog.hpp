#ifndef MWGUI_AICHATDIALOG_H
#define MWGUI_AICHATDIALOG_H

#include "windowbase.hpp"
#include "aichat.hpp"

namespace MyGUI
{
    class EditBox;
    class Button;
    class Widget;
    struct KeyCode;
    typedef unsigned int Char;
}

namespace MWGui
{
    class DialogueWindow;

    class AIChatDialog : public WindowModal
    {
    public:
        AIChatDialog(DialogueWindow* dialogueWindow);
        ~AIChatDialog();

        void onOpen() override;
        void setNpcData(const std::string& name, const std::string& context);
        
        MyGUI::Widget* getDefaultKeyFocus() override;

    protected:
        void onSendClicked(MyGUI::Widget* sender);
        void onCancelClicked(MyGUI::Widget* sender);
        void onKeyPressed(MyGUI::Widget* sender, MyGUI::KeyCode key, MyGUI::Char character);
        void onEditSelectAccept(MyGUI::EditBox* sender);

    private:
        void sendMessage();
        void handleResponse(const std::string& response);

        MyGUI::EditBox* mInputBox;
        MyGUI::Button* mSendButton;
        MyGUI::Button* mCancelButton;
        
        std::string mNpcName;
        std::string mNpcContext;
        
        AIChat mAIChat;
        DialogueWindow* mDialogueWindow;
    };
}

#endif 