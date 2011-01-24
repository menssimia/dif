

#include <dif.h>

#include <Field3D/InitIO.h>

#include <iostream>

using namespace Field3D;


int main(int argc, char *argv[]) {
	initIO();

	Field3DOutputFile ofp;

	if(!ofp.create("test.dif")) {
		std::cout << "Error opening output file" << std::endl;
		return -1;
	}

	V2i size(512, 512);

	DifField<float>::Ptr field = new DifField<float>(size);

	field->name = "somefield";
	field->attribute = "imagefield";

	field->writePixel(V2i(0, 0), V3f(23.0f, 12.0f, 54.0f), 0);

	ofp.writeVectorLayer<float>(field);

	ofp.close();


	// Read the File

	Field3DInputFile ifp;

	if(!ifp.open("test.dif")) {
		std::cout << "Error opening input file" << std::endl;
		return -1;
	}

	Field< FIELD3D_VEC3_T<float> >::Vec rfields = ifp.readVectorLayers<float>();

	if(rfields.size() < 1) {
		std::cout << "OMG What just happend?!??!?!" << std::endl;
		return -1;
	}

	SparseField< FIELD3D_VEC3_T<float> >::Ptr rspfield = field_dynamic_cast< SparseField< FIELD3D_VEC3_T<float> > >(rfields[0]);

	if(!rspfield) {
		std::cout << "Not a sparse field" << std::endl;
		return -1;		
	}

	DifField<float> rfield(*rspfield);

	std::cout << rfield.readPixel(V2i(0, 0), 0) << std::endl;

	return 0;
}
