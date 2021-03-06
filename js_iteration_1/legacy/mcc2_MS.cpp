
#include <cassert>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include "../js_iteration_2/primitives.hpp"
#include "../js_iteration_2/vertex_resampling.hpp"
#include "../js_iteration_2/apply_v_s_to_mc_buffers.hpp"
#include "centroids_projection.cpp"
#include <fstream>
#include "boost/multi_array.hpp"
#include "boost/array.hpp"

#include "debug_methods_MS.hpp"

using namespace std;
using namespace mp5_implicit;

const bool VERBOSE = false;
const bool REPORT_STATS = false;
bool writing_test_file = false;

typedef unsigned short int dim_t; //small integers for example the size of one side of the grid
typedef float REAL;

typedef  boost::multi_array<REAL, 1>  array1d;
typedef boost::array<array1d::index, 1>  array_shape_t;
typedef array1d::index  index_t;

typedef index_t index3_t; //Range of the element type has to be large enough, larger than (size^3)*3.
typedef boost::multi_array<index3_t, 1>   array1d_e3;
typedef std::map<index3_t,int>  e3map_t;


struct callback_t { void call (void*) const { } callback_t(){} };

#include "mcc2_marching_cubes_MS.hpp"

extern "C" {
    void build_geometry(int resolution, REAL mc_size, REAL time);
    int get_v_size();
    int get_f_size();
    void get_f(int*, int);
    void get_v(REAL*, int);
    void finish_geometry();
    void* get_f_ptr();
    void* get_v_ptr();
};


typedef struct {
    bool active = 0;
    MarchingCubes* mc = 0;
} state_t;

state_t _state;


void check_state() {
    if(!_state.active) std::clog << "Error: not active.";
}
void check_state_null() {
    if(_state.active)
        std::clog << "Error: should not be active.";
}

void build_geometry(int resolution, REAL mc_size, REAL time){

    check_state_null();


    bool enableUvs = true;
    bool enableColors = true;

    string name = "cube";
    _state.mc = new MarchingCubes(resolution, mc_size, enableUvs, enableColors);

    _state.mc -> isolation = 0.0;
      // before we had some amazing meatballs! merde a celui qui le lira !


      //********this should become an input of build geometry (and so be set in the html file)*******
      REAL grid_real_size = 1. ; // do not change this


      // f_argument is made to always be between 0. and 1.
      REAL f_argument = 0.5;

    implicit_function * object;

    if (name == "double_mushroom"){
      double_mushroom mushroom(1.4, 0.3, 0.3, 2, 0.1,0.1,0.1); //3.3
      object = &mushroom;
    }
    else if (name == "egg"){
      egg segg(0.3, 0.4, 0.5, 0.2, 0.1, 0.3);
      object = &segg; // super egg !
    }
    else if (name == "torus"){
      torus Tor(2.,0.2,0.2,0.2);
      object = &Tor; // super egg !
    }
    else if (name == "honey_comb"){
      honey_comb honey_comb(0.5, 30., 1.);
      object = &honey_comb; // super egg !
    }
    else if (name == "sphere"){
      unit_sphere sphere(f_argument);
      object = &sphere;
    }
    else if (name == "cube"){
      cube cube(0.6, 0.6, 0.6);
      boost::array<int, 2> direction_shape = { 1, 3 };
      vectorized_vect  direction(direction_shape);
      direction[0][0] = 0.;
      direction[0][1] = 0.0;
      direction[0][2] = 0.3;

    //  cube.rotate(2., direction);
      object = &cube;
    }
    else if (name == "super_bowl"){
      super_bowl super_bowl(0.5, 0.2, 0.3, 0.); //0.5
      object = &super_bowl;
    }

    else if (name == "scone"){
      scone scone(0.8, 0.1, 0.5, 0., 0., 0.5);
      object = &scone;
    }

    else if (name == "lego"){
      legoland legoland(0.8, 0.1, 0.5);
      object = &legoland;
    }
    else if (name == "dice"){
      dice dichu(0.8, 0.1, 0.5);
      object = &dichu;
    }
    else if (name == "heart"){
      heart heartou(0.8, 0.1, 0.5);
      object = &heartou;
    }
    else if (name == "scylinder"){
      boost::array<int, 2> direction_shape = { 1, 3 };
      vectorized_vect  direction(direction_shape);
      direction[0][0] = 0.;
      direction[0][1] = 0.0;
      direction[0][2] = 0.3;
      REAL w[3];
      w[0] = 0;
      w[1] = 1;
      w[2] = 0;
      scylinder scylinder(w , 0.4, 0.4, 0.8, 0., 0.0, 0.0); //0.7
  //    scylinder.rotate(2., direction);
      object = &scylinder;
    }
    else if (name == "egg_cylinder"){

      REAL w[3];
      w[0] = 0;
      w[1] = 1;
      w[2] = 0;
      egg segg(0.6, 0.5, 0.5);
      scylinder scylinder(w, 0.2, 0.1, 0.2);
  //    cube cube(0.4, 0.4, 0.4);
      CrispIntersection crispou(segg, scylinder);
      object = &crispou;
    }
    else if (name == "egg_transform"){
      boost::array<int, 2> direction_shape = { 1, 3 };
      vectorized_vect  direction(direction_shape);
      direction[0][0] = 0.;
      direction[0][1] = 0.3;
      direction[0][2] = 0.3;

      unit_sphere segg(0.5);
      // segg.move(direction);
      // segg.resize(12.);
      segg.rotate(2., direction);
      object = &segg; // super egg !
    }
    else {
      cout << "Error! You must enter a valid name! So I made a sphere!" << endl;
      unit_sphere sphere(f_argument);
      object = &sphere;
     }


    _state.mc->create_shape(object, grid_real_size);

    _state.mc->seal_exterior();

    const callback_t renderCallback;
    _state.mc->render_geometry(renderCallback);

    if(REPORT_STATS){
    int mapctr = 0;
    for (auto& kv_pair: _state.mc->result_e3map){
        if(0)
            std::clog << " [" << kv_pair.first << ':' << kv_pair.second << ']';
        mapctr++;
      }
    }

    // int REPEATS = 2;
    //int REPEATS_VR = 3;
    int REPEATS = 1;
    int REPEATS_VR = 1;
    for (int repeats = 0; repeats < REPEATS ; ++repeats) {
        float c = 2000.;

        for (int i=0; i < REPEATS_VR; i++){
            apply_vertex_resampling_to_MC_buffers__VMS(&object, c, _state.mc->result_verts, _state.mc->result_faces, writing_test_file);
        }

        centroids_projection(object, _state.mc->result_verts, _state.mc->result_faces);
    }


    if(VERBOSE){
        std::clog << resolution << " " << time << std::endl;
        std::clog << _state.mc << std::endl;
    }
    _state.active = true;

    check_state();
}
int get_f_size() {
    check_state();
    return _state.mc->result_faces.size()/3;
}
int get_v_size(){
    check_state();
    return _state.mc->result_verts.size()/3;
}
void get_v(REAL* v_out, int vcount){
    check_state();

    // Vertices
    int ctr = 0;
    for(std::vector<REAL>::iterator it=_state.mc->result_verts.begin(); it < _state.mc->result_verts.end(); it+=3 ){
        for(int di=0; di<3; di++){
            v_out[ctr] = *( it + di );
            ctr++;
        }
    }

    if(vcount*3 != ctr)  std::clog << "sizes dont match: " << (float)ctr/3. << " " << vcount << std::endl;
}

void get_f(int* f_out, int fcount){
    check_state();

    int ctr = 0;
    for(std::vector<int>::iterator it=_state.mc->result_faces.begin(); it < _state.mc->result_faces.end(); it+=3 ){
        for(int di=0; di<3; di++){
            f_out[ctr] = *( it + di );

            ctr++;
        }
    }
    if(fcount*3 != ctr)  std::clog << "sizes dont match: " << (float)ctr/3. << " " << fcount << std::endl;

};

void* get_v_ptr(){
    check_state();
    return (void*)(_state.mc->result_verts.data());
}

void* get_f_ptr(){
    check_state();
    return (void*)(_state.mc->result_faces.data());
}


void finish_geometry() {
    check_state();
    if(_state.mc == 0){
        std::clog << "Error: finish_geometry() before producing the shape()" << std::endl;
    }
    if(!_state.active){

    }
    else{
    }
    delete _state.mc;
    _state.active = false;
    _state.mc = 0;
};


#include "timer.hpp"

int main() {
  //***** This part was used to creat test files *****//
  //
  // if (writing_test_file){
  //
  // int resolution = 28;
  // REAL time = 0.2;
  // check_state_null();
  // bool enableUvs = true;
  // bool enableColors = true;
  //
  // _state.mc = new MarchingCubes(resolution, 1.0, enableUvs, enableColors);
  //
  // _state.mc -> isolation = 0.0;
  //   // before we had some amazing meatballs! merde a celui qui le lira!
  //   REAL grid_real_size= 10;
  // string name = "sphere";
  // _state.mc->create_shape(name,grid_real_size,f_argument);
  //
  // _state.mc->seal_exterior();
  //
  // const callback_t renderCallback;
  // _state.mc->render_geometry(renderCallback);
  //
  //   _state.mc->vertex_resampling(name, f_argument);
  //
  //  }
  //   std::clog << "main();" << std::endl;

    return 0;
}
