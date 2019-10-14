#include <intercept.hpp>

#ifdef _WIN32 
#include <Windows.h>
#include <codecvt>
#endif // _WIN32 


using namespace intercept;

using SQFPar = game_value_parameter;


std::string rawCmdLineString = "";

typedef std::multimap<std::string, std::string> argMulitmap;

argMulitmap argsMap = {};

void readCmdLineArgs() {
#ifdef _WIN32
    LPTSTR cmdline = GetCommandLine();
    rawCmdLineString = std::string(cmdline);


    int argCount;

    auto argArr = CommandLineToArgvW(GetCommandLineW(), &argCount);

    std::vector<std::string> args(argCount);
    for (int i = 0; i < argCount; i++) {
         //= std::wstring(argArr[i]);
         args[i] = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(argArr[i]);
    }
    //std::vector<std::string> args(std::begin(argArr), std::end(argArr));
    



    sqf::diag_log("gacla Output");

    for (int i = 0; i < args.size(); i++) {
        while (args[i].find("\n") != std::string::npos) {
            auto remaining = args[i].substr(args[i].find("\n") + 1);
            args[i].erase(args[i].find("\n"), args[i].size());
            args.push_back(remaining);
        }
    }

    for (int i = 0; i < args.size(); i++) {
        while (std::count(args[i].begin(), args[i].end(), '-') > 1) {
            auto remaining = args[i].substr(args[i].find("-", args[i].find("-") + 1));
            args[i].erase(args[i].find("-", args[i].find("-") + 1), args[i].size());
            args.push_back(remaining);
        }
    }

    for (auto& str : args) {
        if (str.find("-") != std::string::npos) {
            str.erase(str.find("-"), 1);
        }
        /*
        if (str.find("=") != std::string::npos) {
            auto arg = str.substr(0, str.find("="));

            auto values = str.substr(str.find("=") + 1);

            sqf::diag_log("Inital Values:");
            sqf::diag_log(values);

            size_t pos = 0;
            std::string delimiter = "=";
            std::string token;

            while ((pos = values.find(delimiter)) != std::string::npos) {
                token = values.substr(0, pos);
                //sqf::diag_log("Token:");
                sqf::diag_log(token);
                argsMap.insert({ arg, token });
                values.erase(0, pos + delimiter.length());
            }
        }
        else {
            argsMap.insert({ str, "" });
        }*/
        //sqf::diag_log(str);
        if (str.find("=") != std::string::npos) {
            auto arg = str.substr(0, str.find("="));

            auto values = str.substr(str.find("=") + 1);
            argsMap.insert({ arg, values });
        }
        else {
            argsMap.insert({ str, "" });
        }
    }

#else
#endif // _WIN32

}

game_value getRawString(game_state& gs) {
    return rawCmdLineString; // needed?
}

game_value getParameterByArg(game_state& gs, SQFPar rightArg) {
    if (rightArg.type_enum() != game_data_type::STRING) {
        gs.set_script_error(types::game_state::game_evaluator::evaluator_error_type::type, "Should be array"sv);
    }

    //std::string arg = rightArg;

    //auto s = argsMap["map"];

    

    std::pair<argMulitmap::iterator, argMulitmap::iterator> pars = argsMap.equal_range(rightArg);

    auto_array<game_value> ret = {};

    for (argMulitmap::iterator it = pars.first; it != pars.second; it++)
        ret.push_back(it->second);


    return ret;
}

game_value getCmdLineArray(game_state& gs) {

    auto_array<game_value> ret = {};

    for (auto& pair : argsMap) {
        auto_array<game_value> innerArray;
        if (pair.second != "")
            innerArray = { pair.first, pair.second };
        else
            innerArray = { pair.first };
        ret.push_back(innerArray);
    }

    return ret;
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
    static auto gacla_parameter =
        client::host::register_sqf_command("gaclaGetCmdLineParameter", "Returns the Parameter corresponding to the argument", getParameterByArg, game_data_type::ARRAY, game_data_type::STRING);
    static auto gacla_array =
        client::host::register_sqf_command("gaclaGetCmdLineArray", "Returns the Cmd Line String as an array", getCmdLineArray, game_data_type::ARRAY);
}

void intercept::pre_init() {
    intercept::sqf::system_chat("gacla is running!");
}