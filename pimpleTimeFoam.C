/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.0
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Application
    pimpleTimeFoam

Description
    Large time-step transient solver for incompressible, flow using the PIMPLE
    (merged PISO-SIMPLE) algorithm.

    Turbulence modelling is generic, i.e. laminar, RAS or LES may be selected.

    Consistent formulation without time-step and relaxation dependence by Jasak

Author
    Hrvoje Jasak, Wikki Ltd.  All rights reserved

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "singlePhaseTransportModel.H"
#include "turbulenceModel.H"
#include "pimpleControl.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
#   include "setRootCase.H"
#   include "createTime.H"
#   include "createMesh.H"

    pimpleControl pimple(mesh);

#   include "createFields.H"
#   include "initContinuityErrs.H"
#   include "createTimeControls.H"

    Info<< "\nStarting time loop\n" << endl;
		
    #include "StopWatch.H"
    StopWatch totalTime;
	StopWatch mainLoopTime;
	StopWatch turbTime;
	double turbLoopTotal;
	
	// Timing Data file output
	fileName outputFile("timing_log.csv");
	OFstream os(outputFile, ios_base::app);
	if(Pstream::master()){
		os << "Main Loop,Turb Last,Turb Total" << endl;
	}
	
	totalTime.start();

    while (runTime.run())
    {
		mainLoopTime.start();
		
#       include "readTimeControls.H"
#       include "CourantNo.H"
#       include "setDeltaT.H"

        runTime++;

        Info<< "Time = " << runTime.timeName() << nl << endl;

		turbLoopTotal = 0.0;
		
        // --- PIMPLE loop
        while (pimple.loop())
        {
#           include "UEqn.H"

            // --- PISO loop
            while (pimple.correct())
            {
#               include "pEqn.H"
            }
			
			turbTime.start();
			
            turbulence->correct();
			
			turbTime.stop();
			
			turbLoopTotal += turbTime.getIndTime();

        }

        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
			
		mainLoopTime.stop();
		
		// Write to log file
		Info<<   "Main loop time: " << mainLoopTime.getIndTime() << " s"
		<< nl << "Last Iter Turbulence time: " << turbTime.getIndTime() << " s"
		<< nl << "Total Loop Turbulence time: " << turbLoopTotal << " s"
		<< nl << endl;
		
		// Write to data file
		if(Pstream::master()){
			os << mainLoopTime.getIndTime() << ","
			<< turbTime.getIndTime() << ","
			<< turbLoopTotal << endl;		
		}
				

    }
	
	totalTime.stop();
    
	Info<<"Time Profile: "
        <<"\n\tTotal Time (s): " << totalTime.getTotalTime()
	<<endl;

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
