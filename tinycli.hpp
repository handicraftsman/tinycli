namespace TinyCLI {

  class CLIParser {
  private:
    std::vector<std::string> args;

    std::set<char> short_param_wanting;
    std::set<std::string> long_param_wanting;

    std::vector<char> short_args;
    std::vector<std::string> long_args;

    std::map<char, std::string> short_params;
    std::map<std::string, std::string> long_params;

    std::vector<std::string> positional_args;

  public:
    CLIParser(
      int& argc, char**& argv,
      std::set<char> short_param_wanting,
      std::set<std::string> long_param_wanting
    )
    : short_param_wanting(short_param_wanting)
    , long_param_wanting(long_param_wanting)
    {
      for (int i = 1; i < argc; ++i) {
        args.push_back(std::string(argv[i]));
      }
    }

    const std::vector<char>& get_short_args() {
      return short_args;
    }

    const std::vector<std::string>& get_long_args() {
      return long_args;
    }

    const std::map<char, std::string>& get_short_params() {
      return short_params;
    }

    const std::map<std::string, std::string>& get_long_params() {
      return long_params;
    }

    const std::vector<std::string>& get_positional_args() {
      return positional_args;
    }

    void parse() {
      std::optional<std::variant<char, std::string>> wants_param;
      bool takes_args = true;
      for (std::string& arg : args) {
        if (arg[0] == '-' && wants_param && takes_args) {
          throw std::runtime_error("Invalid argument");
        } else if (wants_param && takes_args) {
          auto v = wants_param.value();
          if (auto pval = std::get_if<char>(&v)) {
            short_params[*pval] = arg;
            wants_param = std::nullopt;
          } else if (auto pval = std::get_if<std::string>(&v)) {
            long_params[*pval] = arg;
            wants_param = std::nullopt;
          } else {
            throw std::runtime_error("Invalid variant");
          }
        } else if (arg.size() > 1 && takes_args) {
          if ((arg[0] == '-') && (arg[1] != '-')) {
            for (size_t i = 1; i < arg.size(); ++i) {
              char c = arg[i];
              if (short_param_wanting.find(c) != short_param_wanting.end()) {
                if (i == arg.size() - 1) {
                  wants_param = c;
                } else {
                  short_params[c] = arg.substr(i+1);
                  break;
                }
              } else {
                short_args.push_back(c);
              }
            }
          } else if ((arg[0] == '-') && (arg[1] == '-')) {
            if (arg.size() == 2) {
              takes_args = false;
            } else {
              std::string s = arg.substr(2);
              if (long_param_wanting.find(s) != long_param_wanting.end()) {
                wants_param = s;
              } else {
                long_args.push_back(s);
              }
            }
          } else {
            positional_args.push_back(arg);
          }
        } else {
          positional_args.push_back(arg);
        }
      }
      if (wants_param) {
        throw std::runtime_error("Need another argument");
      }
    }
  };

}
