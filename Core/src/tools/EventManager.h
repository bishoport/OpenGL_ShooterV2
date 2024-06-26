#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <string>


namespace libCore
{
    template <typename... Args>
    class Event {
    public:
        using CallbackType = std::function<void(Args...)>;

        void subscribe(const CallbackType& callback) {
            callbacks.push_back(callback);
        }

        void unsubscribe(const CallbackType& callback) {
            auto it = std::find(callbacks.begin(), callbacks.end(), callback);
            if (it != callbacks.end()) {
                callbacks.erase(it);
            }
        }

        void trigger(Args... args) const {
            for (const auto& callback : callbacks) {
                callback(args...);
            }
        }

    private:
        std::vector<CallbackType> callbacks;
    };




    class EventManager {
    public:

        static Event<int, int>& OnWindowResizeEvent() {
            static Event<int, int> event;
            return event;
        }


        static Event<const std::string&, const glm::vec2&, const  glm::vec2&>& OnPanelResizedEvent() {
            static Event<const std::string&, const  glm::vec2&, const  glm::vec2&> event;
            return event;
        }

    };
}
