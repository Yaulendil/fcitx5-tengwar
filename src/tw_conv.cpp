// #include "tengwar.h"
// #include "wrapper.h"


// std::string mktengwar(std::string input, TengwarMode mode) {
//     char *tengwar;

//     switch (mode) {
//         case TengwarMode::Classical:
//         tengwar = tw_conv_classical(input.data());
//         break;

//         case TengwarMode::Beleriand:
//         tengwar = tw_conv_beleriand(input.data());
//         break;

//         case TengwarMode::Gondor:
//         tengwar = tw_conv_gondor(input.data());
//         break;
//     }

//     std::string clone(tengwar);
//     tw_free(tengwar);

//     return clone;
// }
