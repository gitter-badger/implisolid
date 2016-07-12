'use strict';


function init(service) {
    'use strict';
    //main = Module.cwrap('main', 'number', []);
    //var service={}; //= newProducer //is an interface
    service.build_geometry = Module.cwrap('build_geometry', null, [ 'string', 'string']);
    service.get_v_size = Module.cwrap('get_v_size', 'number', []);
    service.get_f_size = Module.cwrap('get_f_size', 'number', []);
    service.get_v = Module.cwrap('get_v', null, ['number']);
    service.get_f = Module.cwrap('get_f', null, ['number']);
    service.get_v_ptr = Module.cwrap('get_v_ptr', 'number', []);
    service.get_f_ptr = Module.cwrap('get_f_ptr', 'number', []);
    service.finish_geometry = Module.cwrap('finish_geometry', null, []);

    service.init = function(){ service.needsFinish = false; }
    service.finish_with = function (){
        //after the last round.
        if(!this.needsFinish){
            console.error("cannot `finish_geometry()`. Geometry not produced.");
        }
        service.finish_geometry();
        service.needsFinish = false;
    }

    service.init();
    return service;
}


var ImplicitService = function(){
    init(this);
    this.make_geometry = function (shape_params, mc_params) {
        var startTime = new Date();
        const _FLOAT_SIZE = Float32Array.BYTES_PER_ELEMENT;
        const _INT_SIZE = Uint32Array.BYTES_PER_ELEMENT

        if(this.needsFinish) {
            this.finish_geometry();
            this.needsFinish = false;
        }
        this.build_geometry(JSON.stringify(shape_params), JSON.stringify(mc_params));
        this.needsFinish = true;

        var nverts = this.get_v_size();
        var nfaces = this.get_f_size();

        var verts_address = this.get_v_ptr();
        var faces_address = this.get_f_ptr();

        var verts = Module.HEAPF32.subarray(verts_address/_FLOAT_SIZE, verts_address/_FLOAT_SIZE + 3*nverts);
        var faces = Module.HEAPU32.subarray(faces_address/_INT_SIZE, faces_address/_INT_SIZE + 3*nfaces);

        var allocate_buffers = true;
        var geom = new LiveBufferGeometry71(verts, faces, allocate_buffers);

        var endTime = new Date();
        var timeDiff = endTime - startTime;

        //report_time(timeDiff, function(){hist();});

        return geom;
    };
    //This method is called by the designer to obtain the geometry from the ImplicitService 
    this.getLiveGeometry = function(){
        var mc_properties = {resolution: 28, box: {xmin: -1, xmax: 1, ymin: -1, ymax: 1, zmin: -1, zmax: 1}};
        //var shape_properties = {type:"sphere",displayColor:{x:0.38015037447759337,y:0.6015094592616681,z:0.9774198226067741},matrix:[10,0,0,92.9405888205127,0,10,0,101.93969389296757,0,0,10,8.59828143220919,0,0,0,1],index:7935813}
        /*{subjective_time: 0.0, implicit_obj_name: "sphere"*/
        var shape_properties = {type:"meta_balls",time: 0.0};
        var geom = this.make_geometry(shape_properties, mc_properties);
        return geom;
    }

};

var IMPLICIT = null;
function _on_cpp_loaded() {
    console.log("C++ ready.");
    //IMPLISOLID.
    IMPLICIT = new ImplicitService();
};


/* Put the following in the HTML
<script>
        Module={preRun:[],
        onRuntimeInitialized: _on_cpp_loaded,
    };
</script>
<script type="text/javascript" src="mcc2.cpp.js"></script>
*/




function test_update1(t, mesh){
    var g = mesh.geometry;

    IMPLICIT.finish_geometry();
    IMPLICIT.needsFinish = false;

    var mc_properties = {resolution: 28, box: {xmin: -1, xmax: 1+0*Math.sin(t)*3, ymin: -1 , ymax: 1, zmin: -1, zmax: 1}};
    //var new_geometry = IMPLICIT.build_geometry(28, mc_properties, "sphere", 0);

    //var shape_properties = {type: "sphere",matrix:[10,0,0,92.9405888205127,0,10,0,101.93969389296757,0,0,10,8.59828143220919,0,0,0,1]};
    var shape_properties = {type:"meta_balls",time: t };

    var new_geometry = IMPLICIT.build_geometry(
        JSON.stringify(shape_properties) ,
        JSON.stringify(mc_properties));


    IMPLICIT.needsFinish = true;

    if(new_geometry){
        mesh.geometry = new_geometry;
        g = new_geometry;
    }
    g.update_geometry(IMPLICIT);
 
}


function test_update2(t){
    var g = currentMeshes[0].geometry;
    
    var new_geometry = g.update_geometry(IMPLICIT);
    if(new_geometry)
        currentMeshes[0].geometry = new_geometry
}

/*
var t=0;m=currentMeshes[0];test_update1(t, m);var iid=setInterval(function(){test_update1(t+=0.02, m);},6);
*/
