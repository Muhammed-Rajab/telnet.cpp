#ifndef ANSI_COLORS_HPP
#define ANSI_COLORS_HPP

#include <string>

namespace Colors
{
    // Reset
    const std::string Reset = "\033[0m";

    // Text Style
    const std::string Bold = "\033[1m";
    const std::string Underline = "\033[4m";
    const std::string Reversed = "\033[7m"; // Inverted colors
    const std::string Italic = "\033[3m";

    // Text Colors
    const std::string Black = "\033[30m";
    const std::string Red = "\033[31m";
    const std::string Green = "\033[32m";
    const std::string Yellow = "\033[33m";
    const std::string Blue = "\033[34m";
    const std::string Magenta = "\033[35m";
    const std::string Cyan = "\033[36m";
    const std::string White = "\033[37m";

    // Background Colors
    const std::string BG_Black = "\033[40m";
    const std::string BG_Red = "\033[41m";
    const std::string BG_Green = "\033[42m";
    const std::string BG_Yellow = "\033[43m";
    const std::string BG_Blue = "\033[44m";
    const std::string BG_Magenta = "\033[45m";
    const std::string BG_Cyan = "\033[46m";
    const std::string BG_White = "\033[47m";

    // High Intensity (Bright) Colors
    const std::string Bright_Black = "\033[90m";
    const std::string Bright_Red = "\033[91m";
    const std::string Bright_Green = "\033[92m";
    const std::string Bright_Yellow = "\033[93m";
    const std::string Bright_Blue = "\033[94m";
    const std::string Bright_Magenta = "\033[95m";
    const std::string Bright_Cyan = "\033[96m";
    const std::string Bright_White = "\033[97m";

    // Bright Backgrounds
    const std::string BG_Bright_Black = "\033[100m";
    const std::string BG_Bright_Red = "\033[101m";
    const std::string BG_Bright_Green = "\033[102m";
    const std::string BG_Bright_Yellow = "\033[103m";
    const std::string BG_Bright_Blue = "\033[104m";
    const std::string BG_Bright_Magenta = "\033[105m";
    const std::string BG_Bright_Cyan = "\033[106m";
    const std::string BG_Bright_White = "\033[107m";
}

#endif