/*
###############################################################################
# If you use PhysiCell in your project, please cite PhysiCell and the version #
# number, such as below:                                                      #
#                                                                             #
# We implemented and solved the model using PhysiCell (Version x.y.z) [1].    #
#                                                                             #
# [1] A Ghaffarizadeh, R Heiland, SH Friedman, SM Mumenthaler, and P Macklin, #
#     PhysiCell: an Open Source Physics-Based Cell Simulator for Multicellu-  #
#     lar Systems, PLoS Comput. Biol. 14(2): e1005991, 2018                   #
#     DOI: 10.1371/journal.pcbi.1005991                                       #
#                                                                             #
# See VERSION.txt or call get_PhysiCell_version() to get the current version  #
#     x.y.z. Call display_citations() to get detailed information on all cite-#
#     able software used in your PhysiCell application.                       #
#                                                                             #
# Because PhysiCell extensively uses BioFVM, we suggest you also cite BioFVM  #
#     as below:                                                               #
#                                                                             #
# We implemented and solved the model using PhysiCell (Version x.y.z) [1],    #
# with BioFVM [2] to solve the transport equations.                           #
#                                                                             #
# [1] A Ghaffarizadeh, R Heiland, SH Friedman, SM Mumenthaler, and P Macklin, #
#     PhysiCell: an Open Source Physics-Based Cell Simulator for Multicellu-  #
#     lar Systems, PLoS Comput. Biol. 14(2): e1005991, 2018                   #
#     DOI: 10.1371/journal.pcbi.1005991                                       #
#                                                                             #
# [2] A Ghaffarizadeh, SH Friedman, and P Macklin, BioFVM: an efficient para- #
#     llelized diffusive transport solver for 3-D biological simulations,     #
#     Bioinformatics 32(8): 1256-8, 2016. DOI: 10.1093/bioinformatics/btv730  #
#                                                                             #
###############################################################################
#                                                                             #
# BSD 3-Clause License (see https://opensource.org/licenses/BSD-3-Clause)     #
#                                                                             #
# Copyright (c) 2015-2018, Paul Macklin and the PhysiCell Project             #
# All rights reserved.                                                        #
#                                                                             #
# Redistribution and use in source and binary forms, with or without          #
# modification, are permitted provided that the following conditions are met: #
#                                                                             #
# 1. Redistributions of source code must retain the above copyright notice,   #
# this list of conditions and the following disclaimer.                       #
#                                                                             #
# 2. Redistributions in binary form must reproduce the above copyright        #
# notice, this list of conditions and the following disclaimer in the         #
# documentation and/or other materials provided with the distribution.        #
#                                                                             #
# 3. Neither the name of the copyright holder nor the names of its            #
# contributors may be used to endorse or promote products derived from this   #
# software without specific prior written permission.                         #
#                                                                             #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" #
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   #
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  #
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   #
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         #
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        #
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    #
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     #
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     #
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  #
# POSSIBILITY OF SUCH DAMAGE.                                                 #
#                                                                             #
###############################################################################
*/


#include "cancer_metabolism.h"

#include "../modules/PhysiCell_settings.h"

void create_cell_types( void )
{
	// set the random seed 
	SeedRandom( parameters.ints("random_seed") );  
	
	
	  //Put any modifications to default cell definition here if you 
	   //want to have "inherited" by other cell types. 
	   
	  // This is a good place to set default functions. 
	 
	
	initialize_default_cell_definition(); 

	cell_defaults.parameters.o2_proliferation_saturation = 38.0;  
	cell_defaults.parameters.o2_reference = 38.0; 
	
	cell_defaults.functions.update_phenotype = update_cell;  
	cell_defaults.functions.volume_update_function = standard_volume_update_function;
	cell_defaults.functions.update_velocity = NULL;
	cell_defaults.functions.update_migration_bias = NULL; 
	cell_defaults.functions.custom_cell_rule = NULL; 
	
 	
	  // This parses the cell definitions in the XML config file. 
	
	
	initialize_cell_definitions_from_pugixml(); 
	
	
	
	  // Put any modifications to individual cell definitions here. 
	   
	   //This is a good place to set custom functions. 
	 
	
	
	   //This builds the map of cell definitions and summarizes the setup. 
	
		
	build_cell_definitions_maps(); 
	display_cell_definitions( std::cout ); 
	
	return; 
}

void setup_microenvironment( void )
{
	// make sure ot override and go back to 2D 
	if( default_microenvironment_options.simulate_2D == true )
	{
		std::cout << "Warning: overriding 2D setting to return to 3D" << std::endl;
		default_microenvironment_options.simulate_2D = false;
	}
	
	std::vector<double> bc_vector_air( 2 ); // 5% o2
	bc_vector_air[0]=20.0;
	bc_vector_air[1]=0.0;

	
	initialize_microenvironment(); 	
	for( int n = 0; n < microenvironment.mesh.voxels.size() ; n++ )
	{
		
		microenvironment(n)[1] = 0.0;
		microenvironment(n)[2] = 0.0;
		
		
		
	    std::vector<double> position = microenvironment.mesh.voxels[n].center; 
		 if(   position[1] > -220  )
		{	
		microenvironment.add_dirichlet_node( n,bc_vector_air  );
							
		}
	else
		{
			
		// microenvironment.add_dirichlet_node( n,bc_vector_wound );	
		// microenvironment(n)[2] = 1.0; 
			
		}
		//microenvironment(n)[nECM] = 1.0;  
	}
		microenvironment.set_substrate_dirichlet_activation(1,false);
		microenvironment.set_substrate_dirichlet_activation(2,false);
			

	return; 
}

std::vector<std::vector<double>> create_cell_sphere_positions(double cell_radius, double sphere_radius)
{
	std::vector<std::vector<double>> cells;
	int xc=0,yc=0,zc=0;
	double x_spacing= cell_radius*sqrt(3);
	double y_spacing= cell_radius*2;
	double z_spacing= cell_radius*sqrt(3);

	std::vector<double> tempPoint(3,0.0);
	// std::vector<double> cylinder_center(3,0.0);

	for(double z=-sphere_radius;z<sphere_radius;z+=z_spacing, zc++)
	{
		for(double x=-sphere_radius;x<sphere_radius;x+=x_spacing, xc++)
		{
			for(double y=-sphere_radius;y<sphere_radius;y+=y_spacing, yc++)
			{
				tempPoint[0]=x + (zc%2) * 0.5 * cell_radius;
				tempPoint[1]=y + (xc%2) * cell_radius;
				tempPoint[2]=z;

				if(sqrt(norm_squared(tempPoint))< sphere_radius)
				{ cells.push_back(tempPoint); }
			}
		}
	}
	return cells;

}

void setup_tissue( void )
{
	Cell* pC;
	static Cell_Definition* pWound_Def = find_cell_definition("wound_cell");	
	static Cell_Definition* pPA_Def = find_cell_definition("PA");	
	static Cell_Definition* pSA_Def = find_cell_definition("SA");	
	// Wound Cell Seeding

	for (int i=-240; i<250; i+=10)
	{
		pC = create_cell( *pWound_Def ); 
		pC->assign_position( i  , -230, 0.0 );
		pC->is_movable=false;
	}
	
	std::string seeding_method  = parameters.strings("seeding_method");
	
	
    if (seeding_method == "vertical")
    {
         //vertical rows
	for (int i=-220; i<-100; i+=10)
	{
		
		pC = create_cell( *pPA_Def ); 
		pC->assign_position( 10  , i, 0.0 );
		pC = create_cell(*pSA_Def); 
		pC->assign_position( 20  , i, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( 30  , i, 0.0 );
		pC = create_cell(*pSA_Def); 
		pC->assign_position( 40  , i, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( 0 , i, 0.0 );
		pC = create_cell(*pSA_Def); 
		pC->assign_position( -10  , i, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( -20  , i, 0.0 );
		}
	}

    else if (seeding_method == "horizontal")
    {
		// horizontal
		for (int i=-100; i<100; i+=10)
		{
			
		pC = create_cell(*pSA_Def); 
		pC->assign_position( i  , -210, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( i  , -200, 0.0 );
		pC = create_cell(*pSA_Def); 
		pC->assign_position( i  , -190, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( i  , -180, 0.0 );
		}
	}
	
	
	else if (seeding_method == "random")
    {
	     //random
		for (int i=-100; i<100; i+=10)
	{
		for(int j=-210;j<-170;j+=10)
		{
			
			if(rand()-rand()<1)
			{pC = create_cell(*pSA_Def);}
			else
			{pC = create_cell(*pPA_Def);}
			
			pC->assign_position( i  , j, 0.0 );
		}
	
	
	}
	}
	
	
	else
    {
     //box		
		for (int i=-50; i<50; i+=10)
		{
			
		pC = create_cell(*pSA_Def); 
		pC->assign_position( i  , -220, 0.0 );
		pC = create_cell(*pSA_Def); 
		pC->assign_position( i  , -210, 0.0 );
		pC = create_cell(*pSA_Def); 
		pC->assign_position( i  , -200, 0.0 );
		pC = create_cell(*pSA_Def); 
		pC->assign_position( i  , -190, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( i  , -180, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( i  , -170, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( i  , -160, 0.0 );
		}
		
		for (int i=-220; i<-150; i+=10)
		{	
		pC = create_cell(*pPA_Def); 
		pC->assign_position( -60  , i, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( -70 , i, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( 50  , i, 0.0 );
		pC = create_cell(*pPA_Def); 
		pC->assign_position( 60  , i, 0.0 );
		}
	
	}

	
	return; 
}

void update_cell(PhysiCell::Cell* pCell, PhysiCell::Phenotype& phenotype, double dt ){

	dFBAIntracellular *model = (dFBAIntracellular*) phenotype.intracellular;
	model->update(pCell, phenotype, dt);
  
  /*
  phenotype.volume.fluid += dt * phenotype.volume.fluid_change_rate *
  	( phenotype.volume.target_fluid_fraction * phenotype.volume.total - phenotype.volume.fluid );

  if( phenotype.volume.fluid < 0.0 )
  { phenotype.volume.fluid = 0.0; }

  phenotype.volume.cytoplasmic_fluid = phenotype.volume.fluid;

  phenotype.volume.cytoplasmic_solid += dt * phenotype.volume.cytoplasmic_biomass_change_rate *
    (phenotype.volume.target_solid_cytoplasmic - phenotype.volume.cytoplasmic_solid );

  if( phenotype.volume.cytoplasmic_solid < 0.0 )
  { phenotype.volume.cytoplasmic_solid = 0.0; }

  phenotype.volume.solid = phenotype.volume.cytoplasmic_solid;

  phenotype.volume.cytoplasmic = phenotype.volume.cytoplasmic_solid + phenotype.volume.cytoplasmic_fluid;
  phenotype.volume.total = phenotype.volume.cytoplasmic_solid + phenotype.volume.cytoplasmic_fluid;

  // Tell physicell to update the cell radius to the new volume
  phenotype.geometry.update(pCell, phenotype, dt);


*/
}

void setup_default_metabolic_model( void )
{
 	return;
}

void anuclear_volume_model (Cell* pCell, Phenotype& phenotype, double dt)
{
    return;
}

void metabolic_cell_phenotype( Cell* pCell, Phenotype& phenotype, double dt )
{
	// if cell is dead, don't bother with future phenotype changes.
	if( phenotype.death.dead == true )
	{
		pCell->functions.update_phenotype = NULL;
		return;
	}

	// update the transition rate according to growth rate?
	static int cycle_start_index = live.find_phase_index( PhysiCell_constants::live );
	static int cycle_end_index = live.find_phase_index( PhysiCell_constants::live );

	//static int oncoprotein_i = pCell->custom_data.find_variable_index( "oncoprotein" );
	//phenotype.cycle.data.transition_rate( cycle_start_index ,cycle_end_index ) *= pCell->custom_data[oncoprotein_i] ;
	return;
}






std::vector<std::string> my_coloring_function( Cell* pCell )
{
	// start with flow cytometry coloring

	std::vector<std::string> output = false_cell_coloring_cytometry(pCell);
	output[0] = "red";
	output[1] = "red";
	output[2] = "red";

	if( pCell->phenotype.death.dead == false && pCell->type == 1 )
	{
		 output[0] = "black";
		 output[2] = "black";
	}

	return output;
}




void update_Dirichlet_Nodes(void) 

{
	
	
	
	#pragma omp parallel for
	for(int n=0; n < all_cells->size(); n++)
{
			    PhysiCell::Cell* pCell = (*all_cells)[n];
				if (pCell->type==1)
				{
					
					int my_voxel=pCell->get_current_voxel_index();
					microenvironment.remove_dirichlet_node(my_voxel);
					
					
				}

			  }
	
	
	
	/* for( int n = 0; n < microenvironment.mesh.voxels.size() ; n++ )
	{
		
		
		if(  microenvironment.nearest_density_vector( n ) [1] > 1 )
		{	
		
		
		microenvironment.remove_dirichlet_node(n);
		
			
		}

		
	} */
	
	
} 


 void make_adjustments(void)


{
	
	double dnodes=0;
	double leaked_glucose=0.0;
	for( int n = 0; n < microenvironment.mesh.voxels.size() ; n++ )
	{
		if(microenvironment.is_dirichlet_node(n))
		{ 
		dnodes++;
		
			double  glucose_density =microenvironment.nearest_density_vector(n)[2];//[0];
			//std::cout<< glucose_density;
			if(microenvironment.nearest_density_vector(n)[2]>0.0)
			{
			
			std::vector<double> position = microenvironment.mesh.voxels[n].center;
			//std::cout<< microenvironment.mesh.voxels[n].center;
			double offset=20;
			
			std::vector<std::vector<double>> neighbor_voxels(4);
			neighbor_voxels[0]={position[0]+offset,position[1],position[2]};
			neighbor_voxels[1]={position[0],position[1]+offset,position[2]};
			neighbor_voxels[2]={position[0]-offset,position[1],position[2]};
			neighbor_voxels[3]={position[0],position[1]-offset,position[2]};
			
			double non_air=0.0;
			bool check_it[4]={0,0,0,0};
			for (int m=0;m<4;++m)
				
				{ 
					
					if(fabs(neighbor_voxels[m][0])>240||fabs( neighbor_voxels[m][1])>240)
					{	check_it[m]=0;}
					else 
						
					{ 
						if (!(microenvironment.is_dirichlet_node(microenvironment.nearest_voxel_index(neighbor_voxels[m]))))
					{
						non_air+=1.0;
						check_it[m]=1;
					}
				
					}
							
				}
			
			for(int j=0;j<4;j++)
	
			{
				if(check_it[j]==1)
				{
				microenvironment(microenvironment.nearest_voxel_index(neighbor_voxels[j]))[2] += (glucose_density/non_air);
				microenvironment(n)[2]-=(glucose_density/non_air);
					//std::cout<<glucose_density/non_air;
					//std::cout<<non_air;
				}
			//std::cout<<non_air;		
			}
			
			if (non_air==0)
			{
					
				leaked_glucose+=microenvironment.nearest_density_vector(n)[2];
				microenvironment.nearest_density_vector(n)[2]=0;
				
			}
		
		  
		//microenvironment(n)[2]=0;
		}// if glucose found

			
			
		}// end if is_dirichlet_node
		
		
}// end of for
	double total=microenvironment.mesh.voxels.size();

	for( int i = 0; i < microenvironment.mesh.voxels.size() ; i++ )
	{
		
		if (!(microenvironment.is_dirichlet_node(i)))
		{
			
			microenvironment.nearest_density_vector(i)[2]+=(leaked_glucose/(total-dnodes));
			
		}
	
	}
	
}
