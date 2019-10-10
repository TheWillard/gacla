#include <intercept.hpp>

#ifdef _WIN32 
#include <Windows.h>
#include <codecvt>
#endif // _WIN32 


using namespace intercept;

using SQFPar = game_value_parameter;


std::string rawCmdLineString = "";

void readCmdLineArgs() {
#ifdef _WIN32
    LPTSTR cmdline = GetCommandLine();
    rawCmdLineString = std::string(cmdline);
#else
#endif // _WIN32

    int argCount;

    auto argArr = CommandLineToArgvW(GetCommandLineW(), &argCount);

    std::vector<std::string> args(argCount);
    for (int i = 0; i < argCount; i++) {
         //= std::wstring(argArr[i]);
         args[i] = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(argArr[i]);
    }
    //std::vector<std::string> args(std::begin(argArr), std::end(argArr));
    

    std::map<std::string, std::vector<std::string>> argsMap = {};

    sqf::diag_log("gacla Output");
    for (auto& str : args) {
        if (str.find("-") == std::string::npos) {
            str.erase(0, 1);
        }
        if (str.find("=") != std::string::npos) {
            auto arg = str.substr(0, str.find("="));
            
            if (argsMap.count(arg)) {
                argsMap.insert({ arg, {} });
            }

            auto values = str.substr(str.find("=") + 1);

            sqf::diag_log("Inital Values:");
            sqf::diag_log(values);

            size_t pos = 0;
            std::string delimiter = "=";
            std::string token;

            while ((pos = values.find(delimiter)) != std::string::npos) {
                token = values.substr(0, pos);
                sqf::diag_log("Token:");
                sqf::diag_log(token);
                argsMap[arg].push_back(token);
                values.erase(0, pos + delimiter.length());
            }
        }
        else {

        }
        sqf::diag_log(str);
    }

}

game_value getRawString(game_state& gs) {
    return r_string(rawCmdLineString); // needed?
}

int intercept::api_version() { // This is required for the plugin to work.
    return INTERCEPT_SDK_API_VERSION;
}

void intercept::register_interfaces() {
    
}

void intercept::pre_start() {

    readCmdLineArgs();

    /*
        gaclaGetCmdLineString;
        gaclaGetCmdLineArray;
        gaclaGetCmdLineParameter [arg];
    */

    static auto gacla_raw_string =
        client::host::register_sqf_command("gaclaGetCmdLineString", "Returns the raw CmdLine String", getRawString, game_data_type::STRING);
}

void intercept::pre_init() {
    intercept::sqf::system_chat("gacla is running!");
}