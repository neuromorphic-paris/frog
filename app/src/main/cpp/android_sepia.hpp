#pragma once

#include "sepia/source/sepia.hpp"
#include <algorithm>

#include <iostream>
#include <jni.h>

/// android_sepia specialises sepia for the Opal Kelly ATIS.
namespace android_sepia {

    /// camera represents an ATIS connected via USB on an Android telephone.
    class camera {
        public:

        /// width returns the sensor width.
        static constexpr uint16_t width() {
            return 304;
        }

        /// height returns the sensor height.
        static constexpr uint16_t height() {
            return 240;
        }

        camera() = default;
        camera(const camera&) = delete;
        camera(camera&&) = default;
        camera& operator=(const camera&) = delete;
        camera& operator=(camera&&) = default;
        virtual ~camera() {}
    };

    /// specialized_camera represents a template-specialized ATIS connected via USB on an Android telephone.
    template <typename HandleEvent, typename HandleException>
    class specialized_camera : public camera,
                               public sepia::specialized_camera<sepia::dvs_event, HandleEvent, HandleException> {
        public:
        specialized_camera<HandleEvent, HandleException>(
            HandleEvent handle_event,
            HandleException handle_exception,
            std::unique_ptr<sepia::unvalidated_parameter> unvalidated_parameter,
            std::size_t fifo_size,
            JNIEnv *pEnv,
            std::chrono::milliseconds sleep_duration) :
            sepia::specialized_camera<sepia::dvs_event, HandleEvent, HandleException>(
                std::forward<HandleEvent>(handle_event),
                std::forward<HandleException>(handle_exception),
                fifo_size,
                sleep_duration),
            _acquisition_running(true),
            _t_offset(0) {

            // start the reading loop
            _acquisition_loop = std::thread([this, pEnv]() -> void {
                try {
                    sepia::dvs_event event;
                    std::vector<uint8_t> events_data((1 << 24) * 4);
                    std::vector<uint8_t> event_bytes(4);
                    while (_acquisition_running.load(std::memory_order_relaxed)) {
                        if (BUFFER_IS_NOT_EMPTY){
                            if (!this->push(event)) {
                                throw std::runtime_error("JNI's FIFO overflow");
                            }
                        } else {
                            std::this_thread::sleep_for(this->_sleep_duration);
                        }
                    }
                } catch (...) {
                    this->_handle_exception(std::current_exception());
                }
            });
        }
        specialized_camera(const specialized_camera&) = delete;
        specialized_camera(specialized_camera&&) = default;
        specialized_camera& operator=(const specialized_camera&) = delete;
        specialized_camera& operator=(specialized_camera&&) = default;
        virtual ~specialized_camera() {
            _acquisition_running.store(false, std::memory_order_relaxed);
            _acquisition_loop.join();
        }

        protected:
        std::atomic_bool _acquisition_running;
        std::thread _acquisition_loop;
        uint64_t _t_offset;
    };

    /// make_camera creates a camera from functors.
    template <typename HandleEvent, typename HandleException>
    std::unique_ptr<specialized_camera<HandleEvent, HandleException>> make_camera(
        HandleEvent handle_event,
        HandleException handle_exception,
        std::unique_ptr<sepia::unvalidated_parameter> unvalidated_parameter =
            std::unique_ptr<sepia::unvalidated_parameter>(),
        std::size_t fifo_size = 1 << 24,
        JNIEnv *pEnv = nullptr,
        std::chrono::milliseconds sleep_duration = std::chrono::milliseconds(10)) {
        return sepia::make_unique<specialized_camera<HandleEvent, HandleException>>(
            std::forward<HandleEvent>(handle_event),
            std::forward<HandleException>(handle_exception),
            std::move(unvalidated_parameter),
            fifo_size,
            pEnv,
            sleep_duration);
    }
}
