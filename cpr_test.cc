#include "spdlog/fmt/bundled/core.h"
#include <cpr/cpr.h>
#include <glaze/json/write.hpp>
#include <spdlog/spdlog.h>
#include <iostream>
#include <string>
#include <vector>
#include "secrets.h"

const std::string API_URL = "https://openrouter.ai/api/v1/chat/completions";

namespace json = glz;

struct PromptTokensDetails {
    int cached_tokens;
};

struct CompletionTokensDetails {
    int reasoning_tokens;
};

struct Usage {
    int prompt_tokens;
    int completion_tokens;
    int total_tokens;
    PromptTokensDetails prompt_tokens_details;
    CompletionTokensDetails completion_tokens_details;
};

struct ChatMessage {
    std::string role;
    std::string content;
    std::optional<std::string> refusal;
    std::optional<std::string> reasoning;
};

struct ChatRequest {
    std::string model;
    std::vector<ChatMessage> messages;
};

struct ChatChoice {
    std::optional<std::nullptr_t> logprobs;
    std::string finish_reason;
    std::string native_finish_reason;
    int index;
    ChatMessage message;
};

struct ChatResponse {
    std::string id;
    std::string provider;
    std::string model;
    std::string object;
    int64_t created;
    std::vector<ChatChoice> choices;
    std::optional<std::nullptr_t> system_fingerprint;
    Usage usage;
};

template <>
struct glz::meta<PromptTokensDetails> {
    using T = PromptTokensDetails;
    static constexpr auto value = object("cached_tokens", &T::cached_tokens);
};

template <>
struct glz::meta<CompletionTokensDetails> {
    using T = CompletionTokensDetails;
    static constexpr auto value = object("reasoning_tokens", &T::reasoning_tokens);
};

template <>
struct glz::meta<Usage> {
    using T = Usage;
    static constexpr auto value = object(
        "prompt_tokens", &T::prompt_tokens,
        "completion_tokens", &T::completion_tokens,
        "total_tokens", &T::total_tokens,
        "prompt_tokens_details", &T::prompt_tokens_details,
        "completion_tokens_details", &T::completion_tokens_details
    );
};

template <>
struct glz::meta<ChatMessage> {
    using T = ChatMessage;
    static constexpr auto value = object(
        "role", &T::role,
        "content", &T::content,
        "refusal", &T::refusal,
        "reasoning", &T::reasoning
    );
};

template <>
struct glz::meta<ChatRequest> {
    using T = ChatRequest;
    static constexpr auto value = object(
        "model", &T::model,
        "messages", &T::messages
    );
};

template <>
struct glz::meta<ChatChoice> {
    using T = ChatChoice;
    static constexpr auto value = object(
        "logprobs", &T::logprobs,
        "finish_reason", &T::finish_reason,
        "native_finish_reason", &T::native_finish_reason,
        "index", &T::index,
        "message", &T::message
    );
};

template <>
struct glz::meta<ChatResponse> {
    using T = ChatResponse;
    static constexpr auto value = object(
        "id", &T::id,
        "provider", &T::provider,
        "model", &T::model,
        "object", &T::object,
        "created", &T::created,
        "choices", &T::choices,
        "system_fingerprint", &T::system_fingerprint,
        "usage", &T::usage
    );
};

std::string ChatWithOpenRouter(const std::string& message) {
    ChatRequest request{
        .model = "openai/gpt-3.5-turbo",
        .messages = {{.role = "user", .content = message}}
    };

    std::string payload = json::write_json(request);

    cpr::Header headers = {
        {"Authorization", "Bearer " + API_KEY},
        {"Content-Type", "application/json"}
    };

    SPDLOG_INFO("Sending request to OpenRouter...");
    SPDLOG_DEBUG("Payload: {}", payload);

    auto response = cpr::Post(
        cpr::Url{API_URL},
        headers,
        cpr::Body{payload}
    );

    //std::cout << response.text;

    SPDLOG_DEBUG("Response: {}", response.text);

    if (response.status_code != 200) {
        SPDLOG_ERROR("HTTP {}: {}", response.status_code, response.text);
        return "Error: " + std::to_string(response.status_code);
    }

    glz::opts opts;
    // TEMP FIX; INCLUDE ALL KEYS
    opts.error_on_unknown_keys = false;
    ChatResponse parsed;
    if (auto err = json::read_json(parsed, response.text)) {
        SPDLOG_ERROR("Failed to parse JSON: {}", glz::format_error(err, response.text));
        return "Error: Could not parse response.";
    }

    return parsed.choices[0].message.content;
}

int main(int, char**) {
    spdlog::set_pattern("[%D %H:%M:%S.%F] [%^%l%$] [%s %!:%#] [%oms] [%ius]  %v");
    spdlog::set_level(spdlog::level::debug);

    std::string user_input;
    std::cout << "You: ";

    while (std::getline(std::cin, user_input)) {
        try {
            std::string reply = ChatWithOpenRouter(user_input);
            std::cout << "Assistant: " << reply << "\n\nYou: ";
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Exception: {}", e.what());
        }
    }

    return 0;
}