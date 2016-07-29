class MarchingCubesMock {

public:
    MarchingCubesMock( dim_t resolution, bool enableUvs, bool enableColors ) {};
    ~MarchingCubesMock() {}; //why does this have to be public: ?

    //void flush_geometry_queue(std::ostream&);
    void flush_geometry_queue(std::ostream& cout, int& normals_start, std::vector<REAL> &normals,  std::vector<REAL> &verts3, std::vector<int> &faces3, e3map_t &e3map, int& next_unique_vect_counter)
        {};

    inline int polygonize_cube( REAL fx, REAL fy, REAL fz, index_t q, REAL isol, const callback_t& callback ) {return 0;};

    REAL isolation;

//shape:
    void addBall( REAL ballx, REAL bally, REAL ballz, REAL strength, REAL subtract, REAL scale ) {};
    void addPlaneX( REAL strength, REAL subtract ) {};
    void addPlaneZ( REAL strength, REAL subtract ) {};
    void addPlaneY( REAL strength, REAL subtract ) {};
    void seal_exterior(const REAL exterior_value) {};
//field
    void reset() {};

//geometry/threejs interface side.
    void render_geometry(const callback_t& renderCallback ) {};
    void sow() {};

// output. filled using sow()
    int resultqueue_faces_start = 0;
    std::vector<REAL> result_normals;
    std::vector<REAL> result_verts;
    std::vector<int> result_faces;
};
