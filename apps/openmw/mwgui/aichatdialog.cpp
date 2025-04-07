#include "aichatdialog.hpp"
#include "dialogue.hpp"

#include <MyGUI_Button.h>
#include <MyGUI_EditBox.h>
#include <MyGUI_InputManager.h>

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

#include <components/settings/settings.hpp>

namespace MWGui
{
    AIChatDialog::AIChatDialog(DialogueWindow* dialogueWindow)
        : WindowModal("openmw_ai_chat_dialog.layout")
        , mDialogueWindow(dialogueWindow)
    {
        getWidget(mInputBox, "InputBox");
        getWidget(mSendButton, "SendButton");
        getWidget(mCancelButton, "CancelButton");

        // Configure the input box for user input
        mInputBox->setEditStatic(false);
        mInputBox->setEditMultiLine(false);
        mInputBox->setCaption("Type your message here...");
        mInputBox->setTextSelectionColour(MyGUI::Colour::White);
        mInputBox->setEnabled(true);
        mInputBox->setNeedKeyFocus(true);

        // Clear default text when focused
        mInputBox->eventEditSelectAccept += MyGUI::newDelegate(this, &AIChatDialog::onEditSelectAccept);

        // Set up button events
        mSendButton->eventMouseButtonClick += MyGUI::newDelegate(this, &AIChatDialog::onSendClicked);
        mCancelButton->eventMouseButtonClick += MyGUI::newDelegate(this, &AIChatDialog::onCancelClicked);
        
        // Enable sending with Enter key
        mInputBox->eventKeyButtonPressed += MyGUI::newDelegate(this, &AIChatDialog::onKeyPressed);

        // Read API key from settings if available
        std::string apiKey = Settings::Manager::getString("anthropic_api_key", "Game");
        if (!apiKey.empty())
            mAIChat.setApiKey(apiKey);
    }

    void AIChatDialog::onEditSelectAccept(MyGUI::EditBox* sender)
    {
        if (mInputBox->getCaption() == "Type your message here...")
            mInputBox->setCaption("");
    }

    AIChatDialog::~AIChatDialog()
    {
        mAIChat.cancelRequest();
    }

    void AIChatDialog::onOpen()
    {
        WindowModal::onOpen();
        
        // Focus the input box and clear default text
        if (mInputBox && mInputBox->getCaption() == "Type your message here...")
        {
            mInputBox->setCaption("");
        }
            
        center();
        
        // Configure and make the input box visible before setting focus
        if (mInputBox)
        {
            mInputBox->setEditStatic(false);
            mInputBox->setEnabled(true);
            mInputBox->setVisible(true);

            // Force activation of the input box using MyGUI InputManager
            MyGUI::InputManager::getInstance().setKeyFocusWidget(mInputBox);
            
            // Attempt to force focus and cursor visibility
            mInputBox->setTextSelection(0, 0);
            mInputBox->setTextCursor(0);
        }
    }

    void AIChatDialog::setNpcData(const std::string& name, const std::string& context)
    {
        mNpcName = name;
        mNpcContext = context;
    }

    MyGUI::Widget* AIChatDialog::getDefaultKeyFocus()
    {
        return mInputBox;
    }

    void AIChatDialog::onSendClicked(MyGUI::Widget* sender)
    {
        sendMessage();
    }

    void AIChatDialog::onCancelClicked(MyGUI::Widget* sender)
    {
        mAIChat.cancelRequest();
        setVisible(false);
    }

    void AIChatDialog::onKeyPressed(MyGUI::Widget* sender, MyGUI::KeyCode key, MyGUI::Char character)
    {
        if (key == MyGUI::KeyCode::Return || key == MyGUI::KeyCode::NumpadEnter)
        {
            // Don't trigger on Shift+Enter which is used for newlines
            if (!MyGUI::InputManager::getInstance().isShiftPressed())
            {
                sendMessage();
            }
        }
    }

    void AIChatDialog::sendMessage()
    {
        std::string message = mInputBox->getCaption();
        if (message.empty() || message == "Type your message here...")
            return;

        // Disable the send button and show "Sending..." text
        mSendButton->setEnabled(false);
        mSendButton->setCaption("Sending...");

        // Send the message to the AI
        mAIChat.sendMessage(mNpcName, mNpcContext, message, 
            [this](const std::string& response) {
                // Handle the response directly
                handleResponse(response);
            }
        );

        // Close the dialog
        setVisible(false);
    }

    void AIChatDialog::handleResponse(const std::string& response)
    {
        // Re-enable the send button
        mSendButton->setEnabled(true);
        mSendButton->setCaption("Send");

        // Add the response to the dialogue window
        if (mDialogueWindow)
        {
            mDialogueWindow->addResponse(mNpcName, response);
        }
    }
} 