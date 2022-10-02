//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     ConvoDB data = nlohmann::json::parse(jsonString);

#pragma once

#include "nlohmann/json.hpp"

#include <optional>
#include <stdexcept>
#include <regex>

namespace akiJsonConvos {
    using nlohmann::json;

    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }

    class Reply {
        public:
        Reply() = default;
        virtual ~Reply() = default;

        private:
        std::string reply_text;
        int64_t nextexchange;

        public:
        const std::string & get_reply_text() const { return reply_text; }
        std::string & get_mutable_reply_text() { return reply_text; }
        void set_reply_text(const std::string & value) { this->reply_text = value; }

        const int64_t & get_nextexchange() const { return nextexchange; }
        int64_t & get_mutable_nextexchange() { return nextexchange; }
        void set_nextexchange(const int64_t & value) { this->nextexchange = value; }
    };

    class Exchange {
        public:
        Exchange() = default;
        virtual ~Exchange() = default;

        private:
        int64_t id;
        std::string sentence;
        std::vector<Reply> replies;

        public:
        const int64_t & get_id() const { return id; }
        int64_t & get_mutable_id() { return id; }
        void set_id(const int64_t & value) { this->id = value; }

        const std::string & get_sentence() const { return sentence; }
        std::string & get_mutable_sentence() { return sentence; }
        void set_sentence(const std::string & value) { this->sentence = value; }

        const std::vector<Reply> & get_replies() const { return replies; }
        std::vector<Reply> & get_mutable_replies() { return replies; }
        void set_replies(const std::vector<Reply> & value) { this->replies = value; }
    };

    class Conversation {
        public:
        Conversation() = default;
        virtual ~Conversation() = default;

        private:
        int64_t convoid;
        std::vector<Exchange> exchanges;

        public:
        const int64_t & get_convoid() const { return convoid; }
        int64_t & get_mutable_convoid() { return convoid; }
        void set_convoid(const int64_t & value) { this->convoid = value; }

        const std::vector<Exchange> & get_exchanges() const { return exchanges; }
        const Exchange get_exchange_by_id(int id) const {return this->exchanges.at(id-1);}
        std::vector<Exchange> & get_mutable_exchanges() { return exchanges; }
        void set_exchanges(const std::vector<Exchange> & value) { this->exchanges = value; }
    };

    class ConvoDB {
        public:
        ConvoDB() = default;
        virtual ~ConvoDB() = default;

        private:
        std::vector<Conversation> conversations;

        public:
        const std::vector<Conversation> & get_conversations() const { return conversations; }
        const Conversation & get_convo_by_id(int id) {return this->conversations.at(id);}
        std::vector<Conversation> & get_mutable_conversations() { return conversations; }
        void set_conversations(const std::vector<Conversation> & value) { this->conversations = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, akiJsonConvos::Reply & x);
    void to_json(json & j, const akiJsonConvos::Reply & x);

    void from_json(const json & j, akiJsonConvos::Exchange & x);
    void to_json(json & j, const akiJsonConvos::Exchange & x);

    void from_json(const json & j, akiJsonConvos::Conversation & x);
    void to_json(json & j, const akiJsonConvos::Conversation & x);

    void from_json(const json & j, akiJsonConvos::ConvoDB & x);
    void to_json(json & j, const akiJsonConvos::ConvoDB & x);

    inline void from_json(const json & j, akiJsonConvos::Reply& x) {
        x.set_reply_text(j.at("reply_text").get<std::string>());
        x.set_nextexchange(j.at("nextexchange").get<int64_t>());
    }

    inline void to_json(json & j, const akiJsonConvos::Reply & x) {
        j = json::object();
        j["reply_text"] = x.get_reply_text();
        j["nextexchange"] = x.get_nextexchange();
    }

    inline void from_json(const json & j, akiJsonConvos::Exchange& x) {
        x.set_id(j.at("id").get<int64_t>());
        x.set_sentence(j.at("sentence").get<std::string>());
        x.set_replies(j.at("replies").get<std::vector<akiJsonConvos::Reply>>());
    }

    inline void to_json(json & j, const akiJsonConvos::Exchange & x) {
        j = json::object();
        j["id"] = x.get_id();
        j["sentence"] = x.get_sentence();
        j["replies"] = x.get_replies();
    }

    inline void from_json(const json & j, akiJsonConvos::Conversation& x) {
        x.set_convoid(j.at("convoid").get<int64_t>());
        x.set_exchanges(j.at("exchanges").get<std::vector<akiJsonConvos::Exchange>>());
    }

    inline void to_json(json & j, const akiJsonConvos::Conversation & x) {
        j = json::object();
        j["convoid"] = x.get_convoid();
        j["exchanges"] = x.get_exchanges();
    }

    inline void from_json(const json & j, akiJsonConvos::ConvoDB& x) {
        x.set_conversations(j.at("conversations").get<std::vector<akiJsonConvos::Conversation>>());
    }

    inline void to_json(json & j, const akiJsonConvos::ConvoDB & x) {
        j = json::object();
        j["conversations"] = x.get_conversations();
    }
}
