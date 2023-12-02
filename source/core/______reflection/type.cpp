#include "_core.hpp"

namespace madX::Core
{
    void Internal::TypeNameExtract(std::string_view& name) noexcept
    {
        if (name.find(type_probe_) == std::string_view::npos)
        {
            const std::string_view probe_name = TypeName<TypeNameProbe>();
    
            const std::size_t prefix_size = probe_name.find(type_probe_);
            const std::size_t suffix_size = probe_name.length() - prefix_size - type_probe_.length();
            
            name.remove_prefix(prefix_size);
            name.remove_suffix(suffix_size);
        }
    }
}