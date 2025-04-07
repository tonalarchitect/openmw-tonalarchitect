#include "aichat.hpp"

#include <components/misc/strings/format.hpp>

#include <sstream>
#include <curl/curl.h>      // Include libcurl
#include <json/json.h>     // Include jsoncpp

namespace MWGui
{
    AIChat::AIChat()
        : mCancelRequested(false)
    {
    }

    AIChat::~AIChat()
    {
        cancelRequest();
    }

    void AIChat::setApiKey(const std::string& key)
    {
        mApiKey = key;
    }

    bool AIChat::hasApiKey() const
    {
        return !mApiKey.empty();
    }

    void AIChat::cancelRequest()
    {
        // NOTE: We need a way to signal cancellation to curl, but for now
        // this just ensures the thread is joined if it finishes quickly.
        // A more robust implementation would use curl_easy_setopt with
        // CURLOPT_XFERINFOFUNCTION or similar to check mCancelRequested.
        std::lock_guard<std::mutex> lock(mThreadMutex);
        if (mRequestThread.joinable())
        {
            mCancelRequested = true;
            // Don't join immediately, let the request potentially finish/timeout
            // mRequestThread.join(); 
            // mCancelRequested = false;
        }
    }

    void AIChat::sendMessage(const std::string& npcName, const std::string& npcContext,
                           const std::string& playerInput, std::function<void(const std::string&)> callback)
    {
        // Cancel any ongoing request first (thread safe)
        cancelRequest();

        // Reset cancellation flag before starting new request
        mCancelRequested = false;

        // Start the actual API request thread
        std::lock_guard<std::mutex> lock(mThreadMutex);
        mRequestThread = std::thread(&AIChat::performApiRequest, this, npcName, npcContext, playerInput, callback);
        mRequestThread.detach(); 
    }

    // Static callback function for libcurl to write received data into a string
    size_t AIChat::writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    // Method to perform the actual Anthropic API request
    void AIChat::performApiRequest(const std::string& npcName, const std::string& npcContext,
                                   const std::string& playerInput, std::function<void(const std::string&)> callback)
    {
        if (mApiKey.empty())
        {
            callback("Error: API key not set. Please set the API key in the settings.");
            return;
        }

        if (playerInput.empty())
        {
            callback("Error: Please enter a message.");
            return;
        }

        CURL* curl = curl_easy_init();
        if (!curl)
        {
            callback("Error: Failed to initialize network component.");
            return;
        }

        std::string readBuffer;
        std::string responseErrorBuffer;
        responseErrorBuffer.resize(CURL_ERROR_SIZE);

        // Construct JSON request body
        Json::Value requestBody;
        requestBody["model"] = "claude-3-haiku-20240307";
        requestBody["max_tokens"] = 150;
        requestBody["system"] = npcContext; 
        requestBody["messages"][0u]["role"] = "user";
        requestBody["messages"][0u]["content"] = playerInput;

        Json::StreamWriterBuilder writerBuilder;
        writerBuilder["indentation"] = "";
        std::string requestBodyStr = Json::writeString(writerBuilder, requestBody);

        // Set up curl options
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.anthropic.com/v1/messages");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBodyStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, responseErrorBuffer.data()); // Capture curl errors
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); // 30 second timeout

        // Set headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("x-api-key: " + mApiKey).c_str());
        headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");
        headers = curl_slist_append(headers, "content-type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        if (mCancelRequested)
        {
        }
        else if (res != CURLE_OK)
        {
            callback(std::string("Error: Network request failed - ") + curl_easy_strerror(res));
        }
        else
        {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            if (http_code >= 200 && http_code < 300)
            {
                Json::Value responseJson;
                Json::Reader reader;
                bool parsingSuccessful = reader.parse(readBuffer, responseJson);

                if (!parsingSuccessful)
                {
                    callback("Error: Failed to understand API response.");
                }
                else if (responseJson.isObject() && responseJson.isMember("content") && responseJson["content"].isArray() && responseJson["content"].isValidIndex(0u) && responseJson["content"][0u].isMember("text"))
                {
                    std::string generatedText = responseJson["content"][0u]["text"].asString();
                    callback(generatedText);
                }
                else if (responseJson.isObject() && responseJson.isMember("error") && responseJson["error"].isMember("message"))
                {
                     callback("Error: API returned an error - " + responseJson["error"]["message"].asString());
                }
                 else
                {
                    callback("Error: Received unexpected data from API.");
                }
            }
            else
            {
                 callback(std::string("Error: API request failed (Code: ") + std::to_string(http_code) + ")");
            }
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // Remove the mockResponse function entirely
    /*
    void AIChat::mockResponse(const std::string& npcName, const std::string& npcContext,
                            const std::string& playerInput, std::function<void(const std::string&)> callback)
    {
        // ... (old mock code removed) ...
    }
    */
} 