// SPDX-FileCopyrightText: 2022 Contributors to the Power Grid Model project <dynamic.grid.calculation@alliander.com>
//
// SPDX-License-Identifier: MPL-2.0

#include "catch2/catch.hpp"
#include "power_grid_model/auxiliary/input.hpp"
#include "power_grid_model/calculation_parameters.hpp"
#include "power_grid_model/component/sensor.hpp"
#include "power_grid_model/component/voltage_sensor.hpp"
#include "power_grid_model/power_grid_model.hpp"
#include "power_grid_model/three_phase_tensor.hpp"

namespace power_grid_model {

TEST_CASE("Test voltage sensor") {
    SECTION("Test sym/asym calc_param for symmetric voltage sensor, angle = 0") {
        RealValue<true> const u_measured{10.1e3};
        RealValue<true> const u_angle_measured{0};
        double const u_sigma = 1.0;
        double const u_rated = 10.0e3;

        VoltageSensorInput<true> voltage_sensor_input;
        voltage_sensor_input.id = 0;
        voltage_sensor_input.measured_object = 1;
        voltage_sensor_input.u_sigma = u_sigma;
        voltage_sensor_input.u_measured = u_measured;
        voltage_sensor_input.u_angle_measured = u_angle_measured;

        VoltageSensor<true> const voltage_sensor{voltage_sensor_input, u_rated};

        SensorCalcParam<true> const sym_sensor_sym_param = voltage_sensor.calc_param<true>();
        SensorCalcParam<false> const sym_sensor_asym_param = voltage_sensor.calc_param<false>();

        // Test sym voltage sensor with sym param calculation
        CHECK(real(sym_sensor_sym_param.value) == Approx(1.01));
        CHECK(imag(sym_sensor_sym_param.value) == Approx(0.0));
        CHECK(sym_sensor_sym_param.variance == Approx(1.0e-8));

        // Test sym voltage sensor with asym param calculation
        CHECK(real(sym_sensor_asym_param.value[0]) == Approx(1.01));
        CHECK(imag(sym_sensor_asym_param.value[0]) == Approx(0.0));

        CHECK(cabs(sym_sensor_asym_param.value[1]) == Approx(1.01));
        CHECK(arg(sym_sensor_asym_param.value[1]) == Approx(-2 * pi / 3));

        CHECK(cabs(sym_sensor_asym_param.value[2]) == Approx(1.01));
        CHECK(arg(sym_sensor_asym_param.value[2]) == Approx(2 * pi / 3));

        CHECK(sym_sensor_asym_param.variance == Approx(1.0e-8));
    }

    SECTION("Test sym/asym calc_param for symmetric voltage sensor, angle = nan") {
        RealValue<true> const u_measured{10.1e3};
        RealValue<true> const u_angle_measured{nan};
        double const u_sigma = 1.0;
        double const u_rated = 10.0e3;

        VoltageSensorInput<true> voltage_sensor_input;
        voltage_sensor_input.id = 0;
        voltage_sensor_input.measured_object = 1;
        voltage_sensor_input.u_sigma = u_sigma;
        voltage_sensor_input.u_measured = u_measured;
        voltage_sensor_input.u_angle_measured = u_angle_measured;

        VoltageSensor<true> const voltage_sensor{voltage_sensor_input, u_rated};

        SensorCalcParam<true> const sym_sensor_sym_param = voltage_sensor.calc_param<true>();
        SensorCalcParam<false> const sym_sensor_asym_param = voltage_sensor.calc_param<false>();

        // Test sym voltage sensor with sym param calculation
        CHECK(real(sym_sensor_sym_param.value) == Approx(1.01));
        CHECK(is_nan(imag(sym_sensor_sym_param.value)));
        CHECK(sym_sensor_sym_param.variance == Approx(1.0e-8));

        // Test sym voltage sensor with asym param calculation
        CHECK(real(sym_sensor_asym_param.value[0]) == Approx(1.01));
        CHECK(is_nan(imag(sym_sensor_asym_param.value[0])));

        CHECK(real(sym_sensor_asym_param.value[1]) == Approx(1.01));
        CHECK(is_nan(imag(sym_sensor_asym_param.value[1])));

        CHECK(real(sym_sensor_asym_param.value[2]) == Approx(1.01));
        CHECK(is_nan(imag(sym_sensor_asym_param.value[2])));

        CHECK(sym_sensor_asym_param.variance == Approx(1.0e-8));
    }

    SECTION("Test sym/asym calc_param for asymmetric voltage sensor, angle") {
        RealValue<false> const u_measured{10.1e3 / sqrt3, 10.2e3 / sqrt3, 10.3e3 / sqrt3};
        RealValue<false> const u_angle_measured{0.1, (-deg_120 + 0.2), (-deg_240 + 0.3)};
        double const u_sigma = 1.0;
        double const u_rated = 10.0e3;

        VoltageSensorInput<false> voltage_sensor_input;
        voltage_sensor_input.id = 0;
        voltage_sensor_input.measured_object = 1;
        voltage_sensor_input.u_sigma = u_sigma;
        voltage_sensor_input.u_measured = u_measured;
        voltage_sensor_input.u_angle_measured = u_angle_measured;

        VoltageSensor<false> const voltage_sensor{voltage_sensor_input, u_rated};

        SensorCalcParam<true> const asym_sensor_sym_param = voltage_sensor.calc_param<true>();
        SensorCalcParam<false> const asym_sensor_asym_param = voltage_sensor.calc_param<false>();

        // Test asym voltage sensor with sym param calculation
        CHECK(real(asym_sensor_sym_param.value) == Approx((1.01 * cos(0.1) + 1.02 * cos(0.2) + 1.03 * cos(0.3)) / 3));
        CHECK(imag(asym_sensor_sym_param.value) == Approx((1.01 * sin(0.1) + 1.02 * sin(0.2) + 1.03 * sin(0.3)) / 3));
        CHECK(asym_sensor_sym_param.variance == Approx(3.0e-8));

        // Test asym voltage sensor with asym param calculation
        CHECK(cabs(asym_sensor_asym_param.value[0]) == Approx(1.01));
        CHECK(arg(asym_sensor_asym_param.value[0]) == Approx(0.1));

        CHECK(cabs(asym_sensor_asym_param.value[1]) == Approx(1.02));
        CHECK(arg(asym_sensor_asym_param.value[1]) == Approx(-deg_120 + 0.2));

        CHECK(cabs(asym_sensor_asym_param.value[2]) == Approx(1.03));
        CHECK(arg(asym_sensor_asym_param.value[2]) == Approx(deg_120 + 0.3));

        CHECK(asym_sensor_asym_param.variance == Approx(3.0e-8));
    }

    SECTION("Test sym/asym calc_param for asymmetric voltage sensor, angle = nan") {
        RealValue<false> const u_measured{10.1e3 / sqrt3, 10.2e3 / sqrt3, 10.3e3 / sqrt3};
        RealValue<false> const u_angle_measured{nan, nan, nan};
        double const u_sigma = 1.0;
        double const u_rated = 10.0e3;

        VoltageSensorInput<false> voltage_sensor_input;
        voltage_sensor_input.id = 0;
        voltage_sensor_input.measured_object = 1;
        voltage_sensor_input.u_sigma = u_sigma;
        voltage_sensor_input.u_measured = u_measured;
        voltage_sensor_input.u_angle_measured = u_angle_measured;

        VoltageSensor<false> const voltage_sensor{voltage_sensor_input, u_rated};

        SensorCalcParam<true> const asym_sensor_sym_param = voltage_sensor.calc_param<true>();
        SensorCalcParam<false> const asym_sensor_asym_param = voltage_sensor.calc_param<false>();

        // Test asym voltage sensor with sym param calculation
        CHECK(real(asym_sensor_sym_param.value) == Approx((1.01 + 1.02 + 1.03) / 3));
        CHECK(is_nan(imag(asym_sensor_sym_param.value)));
        CHECK(asym_sensor_sym_param.variance == Approx(3.0e-8));

        // Test asym voltage sensor with asym param calculation
        CHECK(real(asym_sensor_asym_param.value[0]) == Approx(1.01));
        CHECK(is_nan(imag(asym_sensor_asym_param.value[0])));

        CHECK(real(asym_sensor_asym_param.value[1]) == Approx(1.02));
        CHECK(is_nan(imag(asym_sensor_asym_param.value[1])));

        CHECK(real(asym_sensor_asym_param.value[2]) == Approx(1.03));
        CHECK(is_nan(imag(asym_sensor_asym_param.value[2])));

        CHECK(asym_sensor_asym_param.variance == Approx(3.0e-8));
    }

    SECTION("Test get_output sym/asym for symmetric voltage sensor") {
        SECTION("Angle = 0") {
            RealValue<true> const u_measured{10.1e3};
            RealValue<true> const u_angle_measured{0};
            double const u_sigma = 1.0;
            double const u_rated = 10.0e3;

            VoltageSensorInput<true> voltage_sensor_input;
            voltage_sensor_input.id = 0;
            voltage_sensor_input.measured_object = 1;
            voltage_sensor_input.u_sigma = u_sigma;
            voltage_sensor_input.u_measured = u_measured;
            voltage_sensor_input.u_angle_measured = u_angle_measured;

            VoltageSensor<true> const voltage_sensor{voltage_sensor_input, u_rated};

            ComplexValue<true> const u_calc_sym{1.02 * exp(1i * 0.2)};
            VoltageSensorOutput<true> sym_voltage_sensor_sym_output = voltage_sensor.get_output<true>(u_calc_sym);

            ComplexValue<false> const u_calc_asym{1.02 * exp(1i * 0.2), 1.03 * exp(1i * 0.3), 1.04 * exp(1i * 0.4)};
            VoltageSensorOutput<false> sym_voltage_sensor_asym_output = voltage_sensor.get_output<false>(u_calc_asym);

            // Check sym output
            CHECK(sym_voltage_sensor_sym_output.id == 0);
            CHECK(sym_voltage_sensor_sym_output.energized == 1);
            CHECK(sym_voltage_sensor_sym_output.u_residual == Approx(-100.0));
            CHECK(sym_voltage_sensor_sym_output.u_angle_residual == Approx(-0.2));

            // Check asym output
            CHECK(sym_voltage_sensor_asym_output.id == 0);
            CHECK(sym_voltage_sensor_asym_output.energized == 1);
            CHECK(sym_voltage_sensor_asym_output.u_residual[0] == Approx(-100.0 / sqrt3));
            CHECK(sym_voltage_sensor_asym_output.u_residual[1] == Approx(-200.0 / sqrt3));
            CHECK(sym_voltage_sensor_asym_output.u_residual[2] == Approx(-300.0 / sqrt3));
            CHECK(sym_voltage_sensor_asym_output.u_angle_residual[0] == Approx(-0.2));
            CHECK(sym_voltage_sensor_asym_output.u_angle_residual[1] == Approx(-0.3));
            CHECK(sym_voltage_sensor_asym_output.u_angle_residual[2] == Approx(-0.4));
        }

        SECTION("Angle = 0.2") {
            RealValue<true> const u_measured{10.1e3};
            RealValue<true> const u_angle_measured{0.2};
            double const u_sigma = 1.0;
            double const u_rated = 10.0e3;

            VoltageSensorInput<true> voltage_sensor_input;
            voltage_sensor_input.id = 0;
            voltage_sensor_input.measured_object = 1;
            voltage_sensor_input.u_sigma = u_sigma;
            voltage_sensor_input.u_measured = u_measured;
            voltage_sensor_input.u_angle_measured = u_angle_measured;

            VoltageSensor<true> const voltage_sensor{voltage_sensor_input, u_rated};

            ComplexValue<true> const u_calc_sym{1.02 * exp(1i * 0.2)};
            VoltageSensorOutput<true> sym_voltage_sensor_sym_output = voltage_sensor.get_output<true>(u_calc_sym);

            ComplexValue<false> const u_calc_asym{1.02 * exp(1i * 0.2), 1.03 * exp(1i * 0.3), 1.04 * exp(1i * 0.4)};
            VoltageSensorOutput<false> sym_voltage_sensor_asym_output = voltage_sensor.get_output<false>(u_calc_asym);

            // Check sym output
            CHECK(sym_voltage_sensor_sym_output.id == 0);
            CHECK(sym_voltage_sensor_sym_output.energized == 1);
            CHECK(sym_voltage_sensor_sym_output.u_residual == Approx(-100.0));
            CHECK(sym_voltage_sensor_sym_output.u_angle_residual == Approx(0.0).margin(1e-12));

            // Check asym output
            CHECK(sym_voltage_sensor_asym_output.id == 0);
            CHECK(sym_voltage_sensor_asym_output.energized == 1);
            CHECK(sym_voltage_sensor_asym_output.u_residual[0] == Approx(-100.0 / sqrt3));
            CHECK(sym_voltage_sensor_asym_output.u_residual[1] == Approx(-200.0 / sqrt3));
            CHECK(sym_voltage_sensor_asym_output.u_residual[2] == Approx(-300.0 / sqrt3));
            CHECK(sym_voltage_sensor_asym_output.u_angle_residual[0] == Approx(0.0).margin(1e-12));
            CHECK(sym_voltage_sensor_asym_output.u_angle_residual[1] == Approx(-0.1));
            CHECK(sym_voltage_sensor_asym_output.u_angle_residual[2] == Approx(-0.2));
        }

        SECTION("Angle = nan") {
            RealValue<true> const u_measured{10.1e3};
            RealValue<true> const u_angle_measured{nan};
            double const u_sigma = 1.0;
            double const u_rated = 10.0e3;

            VoltageSensorInput<true> voltage_sensor_input;
            voltage_sensor_input.id = 0;
            voltage_sensor_input.measured_object = 1;
            voltage_sensor_input.u_sigma = u_sigma;
            voltage_sensor_input.u_measured = u_measured;
            voltage_sensor_input.u_angle_measured = u_angle_measured;

            VoltageSensor<true> const voltage_sensor{voltage_sensor_input, u_rated};

            ComplexValue<true> const u_calc_sym{1.02 * exp(1i * 0.2)};
            VoltageSensorOutput<true> sym_voltage_sensor_sym_output = voltage_sensor.get_output<true>(u_calc_sym);

            ComplexValue<false> const u_calc_asym{1.02 * exp(1i * 0.2), 1.03 * exp(1i * 0.3), 1.04 * exp(1i * 0.4)};
            VoltageSensorOutput<false> sym_voltage_sensor_asym_output = voltage_sensor.get_output<false>(u_calc_asym);

            // Check sym output
            CHECK(sym_voltage_sensor_sym_output.id == 0);
            CHECK(sym_voltage_sensor_sym_output.energized == 1);
            CHECK(sym_voltage_sensor_sym_output.u_residual == Approx(-100.0));
            CHECK(is_nan(sym_voltage_sensor_sym_output.u_angle_residual));

            // Check asym output
            CHECK(sym_voltage_sensor_asym_output.id == 0);
            CHECK(sym_voltage_sensor_asym_output.energized == 1);
            CHECK(sym_voltage_sensor_asym_output.u_residual[0] == Approx(-100.0 / sqrt3));
            CHECK(sym_voltage_sensor_asym_output.u_residual[1] == Approx(-200.0 / sqrt3));
            CHECK(sym_voltage_sensor_asym_output.u_residual[2] == Approx(-300.0 / sqrt3));
            CHECK(is_nan(sym_voltage_sensor_asym_output.u_angle_residual[0]));
            CHECK(is_nan(sym_voltage_sensor_asym_output.u_angle_residual[1]));
            CHECK(is_nan(sym_voltage_sensor_asym_output.u_angle_residual[2]));
        }
    }

    SECTION("Test get_output sym/asym for asymmetric voltage sensor") {
        SECTION("With angle") {
            RealValue<false> const u_measured{
                10.1e3 / sqrt3, 10.2e3 / sqrt3,
                10.3e3 / sqrt3};  // Asym voltage sensor measures line-ground voltage, hence /sqrt3
            RealValue<false> const u_angle_measured{0.1, 0.2, 0.3};
            double const u_sigma = 1.0;
            double const u_rated = 10.0e3;

            VoltageSensorInput<false> voltage_sensor_input;
            voltage_sensor_input.id = 0;
            voltage_sensor_input.measured_object = 1;
            voltage_sensor_input.u_sigma = u_sigma;
            voltage_sensor_input.u_measured = u_measured;
            voltage_sensor_input.u_angle_measured = u_angle_measured;

            VoltageSensor<false> const voltage_sensor{voltage_sensor_input, u_rated};

            ComplexValue<true> const u_calc_sym{1.02 * exp(1i * 0.2)};
            VoltageSensorOutput<true> asym_voltage_sensor_sym_output = voltage_sensor.get_output<true>(u_calc_sym);
            DoubleComplex const u1_measured = voltage_sensor.calc_param<true>().value;

            ComplexValue<false> const u_calc_asym{1.02 * exp(1i * 0.2), 1.04 * exp(1i * 0.4), 1.06 * exp(1i * 0.6)};
            VoltageSensorOutput<false> asym_voltage_sensor_asym_output = voltage_sensor.get_output<false>(u_calc_asym);

            // Check sym output
            CHECK(asym_voltage_sensor_sym_output.id == 0);
            CHECK(asym_voltage_sensor_sym_output.energized == 1);
            CHECK(asym_voltage_sensor_sym_output.u_residual ==
                  Approx((cabs(u1_measured) - cabs(u_calc_sym)) * u_rated));
            CHECK(asym_voltage_sensor_sym_output.u_angle_residual == Approx(arg(u1_measured) - 0.2));

            // Check asym output
            CHECK(asym_voltage_sensor_asym_output.id == 0);
            CHECK(asym_voltage_sensor_asym_output.energized == 1);
            CHECK(asym_voltage_sensor_asym_output.u_residual[0] == Approx(-100.0 / sqrt3));
            CHECK(asym_voltage_sensor_asym_output.u_residual[1] == Approx(-200.0 / sqrt3));
            CHECK(asym_voltage_sensor_asym_output.u_residual[2] == Approx(-300.0 / sqrt3));
            CHECK(asym_voltage_sensor_asym_output.u_angle_residual[0] == Approx(-0.1));
            CHECK(asym_voltage_sensor_asym_output.u_angle_residual[1] == Approx(-0.2));
            CHECK(asym_voltage_sensor_asym_output.u_angle_residual[2] == Approx(-0.3));
        }

        SECTION("Angle = nan") {
            RealValue<false> const u_measured{
                10.1e3 / sqrt3, 10.2e3 / sqrt3,
                10.3e3 / sqrt3};  // Asym voltage sensor measures line-ground voltage, hence /sqrt3
            RealValue<false> const u_angle_measured{nan, nan, nan};
            double const u_sigma = 1.0;
            double const u_rated = 10.0e3;

            VoltageSensorInput<false> voltage_sensor_input;
            voltage_sensor_input.id = 0;
            voltage_sensor_input.measured_object = 1;
            voltage_sensor_input.u_sigma = u_sigma;
            voltage_sensor_input.u_measured = u_measured;
            voltage_sensor_input.u_angle_measured = u_angle_measured;

            VoltageSensor<false> const voltage_sensor{voltage_sensor_input, u_rated};

            ComplexValue<true> const u_calc_sym{1.02 * exp(1i * 0.2)};
            VoltageSensorOutput<true> asym_voltage_sensor_sym_output = voltage_sensor.get_output<true>(u_calc_sym);

            ComplexValue<false> const u_calc_asym{1.02 * exp(1i * 0.2), 1.04 * exp(1i * 0.4), 1.06 * exp(1i * 0.6)};
            VoltageSensorOutput<false> asym_voltage_sensor_asym_output = voltage_sensor.get_output<false>(u_calc_asym);

            // Check sym output
            CHECK(asym_voltage_sensor_sym_output.id == 0);
            CHECK(asym_voltage_sensor_sym_output.energized == 1);
            CHECK(asym_voltage_sensor_sym_output.u_residual == Approx(0.0).margin(1e-6));
            CHECK(is_nan(asym_voltage_sensor_sym_output.u_angle_residual));

            // Check asym output
            CHECK(asym_voltage_sensor_asym_output.id == 0);
            CHECK(asym_voltage_sensor_asym_output.energized == 1);
            CHECK(asym_voltage_sensor_asym_output.u_residual[0] == Approx(-100.0 / sqrt3));
            CHECK(asym_voltage_sensor_asym_output.u_residual[1] == Approx(-200.0 / sqrt3));
            CHECK(asym_voltage_sensor_asym_output.u_residual[2] == Approx(-300.0 / sqrt3));
            CHECK(is_nan(asym_voltage_sensor_asym_output.u_angle_residual[0]));
            CHECK(is_nan(asym_voltage_sensor_asym_output.u_angle_residual[1]));
            CHECK(is_nan(asym_voltage_sensor_asym_output.u_angle_residual[2]));
        }
    }
}

}  // namespace power_grid_model
