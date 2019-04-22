/* 
 * File:   breakpoints.h
 * Author: goodwyna21
 *
 * Created on February 15, 2019, 8:55 AM
 */

#ifndef BREAKPOINTS_H
#define BREAKPOINTS_H

int BKPCNT=1;

void bp(){           
    std::cout << "\n------------\nBreakpoint " << BKPCNT << "\n------------\n";
    std::cin.get();
    BKPCNT++;
}

void bp(std::string msg){
    std::cout << "\n------------\nBreakpoint " << BKPCNT << "\n" << ((msg=="")?"empty str":msg) << "\n------------\n";
    std::cin.get();
    BKPCNT++;
}

/*void bp(bool b){
    std::cout << "\n------------\nBreakpoint " << BKPCNT << "\n" << (b?"True":"False") << "\n------------\n";
    std::cin.get();
    BKPCNT++;
}*/

void bp(int n){
    std::cout << "\n------------\nBreakpoint " << BKPCNT << "\n" << n << "\n------------\n";
    std::cin.get();
    BKPCNT++;
}

void bp(std::string * n){
    std::cout << "\n------------\nBreakpoint " << BKPCNT << "\n" << n << "\n------------\n";
    std::cin.get();
    BKPCNT++;
}

void bp(char c){
    std::cout << "\n------------\nBreakpoint " << BKPCNT << "\n" << c << "\n------------\n";
    std::cin.get();
    BKPCNT++;
}

#endif /* BREAKPOINTS_H */

