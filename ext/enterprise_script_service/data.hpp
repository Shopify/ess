#ifndef ENTERPRISE_SCRIPT_SERVICE_DATA_HPP
#define ENTERPRISE_SCRIPT_SERVICE_DATA_HPP

#include <msgpack.hpp>


static const int SYMBOL_EXT_CODE = 0x00;

class symbol {
  std::string name_;

public:
  symbol(std::string name) : name_(name) { };
  const std::string &name() const { return name_; }
};

namespace msgpack {
  MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    namespace adaptor {
      template<>
      struct pack<symbol> {
        template <typename Stream>
        msgpack::packer<Stream> &operator()(msgpack::packer<Stream> &packer, const symbol &s) const {
          auto &name = s.name();
          packer.pack_ext(name.size(), SYMBOL_EXT_CODE);
          packer.pack_ext_body(name.data(), name.size());
          return packer;
        }
      };
    }
  }
}

struct output_stream {
  output_stream(const int fd);
  output_stream &write(const char *, std::size_t);

  const int fd;
};


using out_packer = msgpack::packer<output_stream>;

class data_writer {
public:
  data_writer(out_packer &packer);
  void emit_measurement(std::string key, int64_t value) noexcept;

  out_packer &packer;
};


#endif
