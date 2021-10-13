PA_SBML = readCbModel('PA_iMO1056.xml');
PA_Solution = optimizeCbModel(PA_SBML);


SA_SBML = readCbModel('SA_iYS854.xml');
SA_Solution = optimizeCbModel(SA_SBML);