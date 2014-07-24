/* ----------------------------------------------------------------------- *//**
 *
 * @file link.hpp
 *
 *//* ----------------------------------------------------------------------- */


#ifndef MADLIB_MODULES_GLM_LINK_HPP
#define MADLIB_MODULES_GLM_LINK_HPP

#include <cmath>
#include <modules/prob/boost.hpp>
#include <boost/math/distributions.hpp>
#include <boost/math/special_functions/erf.hpp>

// ------------------------------------------------------------

using namespace madlib::dbal::eigen_integration;

namespace madlib {

namespace modules {

namespace glm {

using namespace std;

// ------------------------------------------------------------

class Identity {
public:
    static double init(const double &y) { return y; }
    static double link_func(const double &mu) { return mu; }
    static double mean_func(const double &ita) { return ita; }
    static double mean_derivative(const double &) { return 1.; }
};

// ------------------------------------------------------------

class Log {
public:
    static double init(const double &y) { return std::max(y, 0.1); }
    static double link_func(const double &mu) { return log(mu); }
    static double mean_func(const double &ita) { return exp(ita); }
    static double mean_derivative(const double &ita) { return exp(ita); }
};

// ------------------------------------------------------------

class Sqrt {
public:
    static double init(const double &y) { return std::max(y, 0.); }
    static double link_func(const double &mu) { return sqrt(mu); }
    static double mean_func(const double &ita) { return ita * ita; }
    static double mean_derivative(const double &ita) { return 2 * ita; }
};

// ------------------------------------------------------------
class Inverse {
public:
    static double init(const double &y) { return y == 0 ? 0.1 : y; }
    static double link_func(const double &mu) { return 1./mu; }
    static double mean_func(const double &ita) { return 1./ita; }
    static double mean_derivative(const double &ita) { return -1./(ita*ita); }
};

// ------------------------------------------------------------

class SqrInverse {
public:
    static double init(const double &y) { return y == 0 ? 0.1 : y; }
    static double link_func(const double &mu) { return 1./mu/mu; }
    static double mean_func(const double &ita) { return 1./sqrt(ita); }
    static double mean_derivative(const double &ita) { return -1./2/sqrt(ita*ita*ita); }
};

// ------------------------------------------------------------

class Probit
{
public:
    static double init(const double &y) { return (y + 0.5) / 2; }
    static double link_func(const double &mu) {
        double root_2 = sqrt(2);
        return root_2 * boost::math::erf_inv(2*mu-1);
    }

    static double mean_func(const double &ita) {
        return prob::cdf(prob::normal(), ita);
    }

    static double mean_derivative(const double &ita) {
        return exp(-ita*ita/2.)/sqrt(2*M_PI);
    }
};

// ------------------------------------------------------------

class Logit {
public:
    static double init(const double &y) { return (y + 0.5) / 2; }
    static double link_func(const double &mu) {
        return log(mu / (1 - mu));
    }
    static double mean_func(const double &ita) {
        return 1./(1 + exp(-ita));
    }
    static double mean_derivative(const double &ita) {
        return 1./((1 + exp(-ita)) * (1 + exp(ita)));
    }
};

// ------------------------------------------------------------

class MultiLogit {
public:
    static void init(ColumnVector &mu) {
        mu.fill(1./static_cast<double>(mu.size()+1)); // later we may consider to use y to initialize mu
    }
    static void link_func(const ColumnVector &mu, ColumnVector &ita) {
        for (int i=0;i<mu.size();i++) {
          ita(i) = log(mu(i)) - log(1-mu.sum());
        }
    }
    static void mean_func(const ColumnVector &ita, ColumnVector &mu) {
        double temp=0;
        for(int i=0;i<ita.size();i++) {
           temp += exp(ita(i));
        }
        temp = temp+1;
        for(int i=0;i<ita.size();i++) {
           mu(i) = exp(ita(i))/temp;
        }
    }
    static void mean_derivative(const ColumnVector &ita, Matrix &mu_prime) {
        double temp=0;
        for(int i=0;i<ita.size();i++) {
           temp += exp(ita(i));
        }
        temp = temp+1;

        for(int i=0;i<ita.size();i++) {
          for(int j=0;j<ita.size();j++) {
             if(i==j) { mu_prime(i,j)=exp(ita(i))*(temp - exp(ita(i)))/(temp*temp); }
             else { mu_prime(i,j)=-exp(ita(i))*exp(ita(j))/(temp*temp); }
          }
        }
    }
};

} // namespace glm

} // namespace modules

} // namespace madlib

#endif // defined(MADLIB_MODULES_GLM_LINK_HPP)