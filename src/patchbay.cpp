#include "patchbay.impl.hpp"
#include "logger.hpp"

#include <future>
#include <optional>

#include <glaze/glaze.hpp>

namespace vencord
{
    patchbay::~patchbay() = default;

    patchbay::patchbay() : m_impl(std::make_unique<impl>()) {}

    void patchbay::link(link_options options)
    {
        logger::get()->trace(R"([patchbay] (link) request: "{}")", glz::write_json(options));
        m_impl->sender->send(std::move(options));
    }

    void patchbay::unlink()
    {
        logger::get()->trace("[patchbay] unlink requested");
        m_impl->sender->send(unset_target{});
    }

    std::vector<node> patchbay::list(std::vector<std::string> props)
    {
        m_impl->sender->send(list_nodes{std::move(props)});
        return m_impl->receiver->recv_as<std::vector<node>>();
    }

    patchbay &patchbay::get()
    {
        static std::unique_ptr<patchbay> instance;

        if (!has_pipewire())
        {
            throw std::runtime_error("not running pipewire");
        }

        if (!instance)
        {
            instance = std::unique_ptr<patchbay>(new patchbay);
            logger::get()->info("[patchbay] (get) running venmic {}", VENMIC_VERSION);
        }

        return *instance;
    }

    bool patchbay::has_pipewire()
    {
        return true;
    }
} // namespace vencord
