#ifndef MWGUI_AICHAT_H
#define MWGUI_AICHAT_H

#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>

// Forward declarations for libraries
struct curl_slist;
namespace Json { class Value; }

namespace MWGui
{
    /**
     * Mock implementation of AI chat API
     */
    class AIChat
    {
    public:
        AIChat();
        ~AIChat();

        /**
         * Send a message to the AI and get a response
         * @param npcName The name of the NPC being spoken to
         * @param npcContext Additional context about the NPC (race, faction, etc.)
         * @param playerInput The text input from the player
         * @param callback Function to call with the response text
         */
        void sendMessage(const std::string& npcName, const std::string& npcContext, 
                         const std::string& playerInput, std::function<void(const std::string&)> callback);
        
        /**
         * Cancels any ongoing request
         */
        void cancelRequest();

        /**
         * Sets the API key
         */
        void setApiKey(const std::string& key);
        
        /**
         * Returns true if API key is set
         */
        bool hasApiKey() const;

    private:
        /**
         * Performs the actual API request in a background thread.
         */
        void performApiRequest(const std::string& npcName, const std::string& npcContext, 
                               const std::string& playerInput, std::function<void(const std::string&)> callback);

        // Static helper for curl write callback
        static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

        std::string mApiKey;
        std::thread mRequestThread;
        std::atomic<bool> mCancelRequested;
        std::mutex mThreadMutex;
    };
}

#endif 