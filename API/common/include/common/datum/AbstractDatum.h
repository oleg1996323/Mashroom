#pragma once
#include <string_view>
#include <system_error>
#include <memory>
#include <limits>
#include <cmath>

namespace datum{
    enum class type{
        spheric,
        elliptic,
        geoid
    };

    class AbstractDatum{
        std::string_view name_;
        protected:
        AbstractDatum(std::string_view name):name_(name){}
        public:
        virtual ~AbstractDatum() = default;
        AbstractDatum(const AbstractDatum&) = delete;
        AbstractDatum(AbstractDatum&&) noexcept = delete;
        AbstractDatum& operator=(const AbstractDatum&) noexcept = delete;
        AbstractDatum& operator=(AbstractDatum&&) noexcept = delete;
        virtual double a() const noexcept = 0;
        virtual double b() const noexcept = 0;
        virtual double f() const noexcept = 0;
        bool flattened() const noexcept{
            return f()>std::numeric_limits<double>::epsilon();
        }
        double first_eccentricity() const noexcept{
            return std::sqrt(first_eccentricity2());
        }
        double first_eccentricity2() const noexcept{
            return 2*f()-f()*f();
        }
        double second_eccentricity() const noexcept{
            double fl = f(); 
            return fl/(2.0 - fl); 
        }
        std::string_view name() const noexcept{
            return name_;
        }
        virtual type datum_type() const noexcept = 0;
    };

    class SphericDatum:public AbstractDatum{
        // Большая полуось (экваториальный радиус) в метрах
        double a_;
        public:
        SphericDatum(
            std::string_view name,
            double a):
            AbstractDatum(std::move(name)),
            a_(a){}
        virtual ~SphericDatum() = default;
        double a() const noexcept final{
            return a_;
        }
        double b() const noexcept final{
            return a_;
        }
        virtual double f() const noexcept final{
            return 0;
        }
        virtual type datum_type() const noexcept final{
            return type::spheric;
        }
    };

    class EllipticDatum:public AbstractDatum{
        // Большая полуось (экваториальный радиус) в метрах
        double a_;
        // Сжатие (flattening)    
        double f_;        
        public:
        EllipticDatum(std::string_view name,
                double a,
                double f):
            AbstractDatum(std::move(name)),
            a_(a),
            f_(f){}
        virtual ~EllipticDatum() = default;
        virtual type datum_type() const noexcept final{
            return type::elliptic;
        }
        double a() const noexcept final{
            return a_;
        }
        double b() const noexcept final{
            return (1.-f())*a_;
        }
        double f() const noexcept final{
            return f_;
        }
    };

    class GeoidDatum:public AbstractDatum{
        // Большая полуось (экваториальный радиус) в метрах
        double a_;
        // Сжатие (flattening)    
        double f_;
        protected:

        public:
        GeoidDatum(std::string_view name,
                double a,
                double f):
            AbstractDatum(std::move(name)),
            a_(a),
            f_(f){}
        virtual ~GeoidDatum() = default;
        virtual type datum_type() const noexcept final{
            return type::geoid;
        }
        double a() const noexcept final{
            return a_;
        }
        double b() const noexcept final{
            return (1.-f())*a_;
        }
        double f() const noexcept final{
            return f_;
        }
    };

    std::error_code register_datum(std::string name, double a,double f,type t) noexcept;
    std::shared_ptr<AbstractDatum> get_datum(std::string_view name) noexcept;
}