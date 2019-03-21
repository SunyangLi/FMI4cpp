/*
 * The MIT License
 *
 * Copyright 2017-2018 Norwegian University of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING  FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string>
#include <iostream>

#include <fmi4cpp/fmi4cpp.hpp>

using namespace std;
using namespace fmi4cpp;

const double stop = 0.01;
const double stepSize = 1E-3;

const string fmuPath = "../resources/fmus/2.0/cs/20sim/4.6.4.8004/"
                       "ControlledTemperature/ControlledTemperature.fmu";

int main() {

    fmi2::fmu fmu(fmuPath);
    auto cs_fmu = fmu.as_cs_fmu();
    auto md = cs_fmu->model_description();

    auto var = md->modelVariables->getByValueReference(47).as_real();
    cout << "Name=" << var.name() << ", start=" << var.start().value_or(0) << endl;

    auto slave1 = cs_fmu->new_instance();
    auto slave2 = cs_fmu->new_instance();

    cout << "modelIdentifier=" << slave1->model_description()->modelIdentifier << endl;

    slave1->setupExperiment();
    slave1->enterInitializationMode();
    slave1->exitInitializationMode();

    slave2->setupExperiment();
    slave2->enterInitializationMode();
    slave2->exitInitializationMode();

    vector<fmi2Real> ref(2);
    vector<fmi2ValueReference> vr = {md->getVariableByName("Temperature_Reference").valueReference,
                                     md->getVariableByName("Temperature_Room").valueReference};

    double t = 0;
    while ((t = slave1->getSimulationTime()) <= stop) {

        if (!slave1->step(stepSize)) { break; }
        if (!slave1->readReal(vr, ref)) { break; }
        cout << "t=" << t << ", Temperature_Reference=" << ref[0] << ", Temperature_Room=" << ref[1] << endl;

    }

    cout << "FMU '" << fmu.model_name() << "' terminated with success: " << (slave1->terminate() == 1 ? "true" : "false") << endl;
    cout << "FMU '" << fmu.model_name() << "' terminated with success: " << (slave2->terminate() == 1 ? "true" : "false") << endl;

    return 0;

}