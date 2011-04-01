

#include <dif.h>

#include <Field3D/InitIO.h>

#include <iostream>

using namespace Field3D;

int fieldtest() {
	Field3DOutputFile ofp;

	if(!ofp.create("test.dif")) {
		std::cout << "Error opening output file" << std::endl;
		return -1;
	}

	V2i size(4096, 1714);

	DifField<float>::Ptr field = new DifField<float>(size);

	field->name = "somefield";
	field->attribute = "imagefield";

	std::cout << 0 << std::endl;
	field->writePixel(V2i(0, 0), 1, 3.05f);
	field->writePixel(V2i(1, 0), 0, 3.04f);
	std::cout << 1 << std::endl;
	std::cout << 1 << std::endl;
	field->writePixel(V2i(6, 2), 1, 3.08f);
	std::cout << 0 << std::endl;
	field->writePixel(V2i(52, 210), 0, 3.04f);

	ofp.writeScalarLayer<float>(field);

	ofp.close();

	// readPixel the File

	Field3DInputFile ifp;

	if(!ifp.open("test.dif")) {
		std::cout << "Error opening input file" << std::endl;
		return -1;
	}

	Field<float>::Vec rfields = ifp.readScalarLayers<float>();

	if(rfields.size() < 1) {
		std::cout << "OMG What just happend?!?!?!" << std::endl;
		return -1;
	}

	SparseField<float>::Ptr rspfield = field_dynamic_cast< SparseField<float> >(rfields[0]);

	if(!rspfield) {
		std::cout << "Not a sparse field" << std::endl;
		return -1;		
	}

	DifField<float> rfield(*rspfield);

	bool retval = false;

	std::cout << rfield.getSize() << std::endl;

	std::cout << rfield.readPixel(V2i(0, 0), 1, &retval);
	std::cout << (retval ? "true" : "false") << std::endl;

	std::cout << rfield.readPixel(V2i(1, 0), 0, &retval);
	std::cout << (retval ? "true" : "false") << std::endl;

	ofp.close();
	ifp.close();

}

int main(int argc, char *argv[]) {
	initIO();

	fieldtest();

	DifImage<float> dif(V2i(12,12));

	unsigned int r, g, b, a;
	dif.addChannel("r", r);
	dif.addChannel("g", g);
	dif.addChannel("b", b);
	dif.addChannel("a", a);


	float data[4] = {2.0f, 1.0f, 3.0f, 4.0f};

	dif.writeData(V2i(0,0), 0.0f, data);

	data[2] = 12.1;

	dif.writeData(V2i(0,0), 23.1f, data);

	float odata[4];

	dif.readData(V2i(0,0), 0.0f, odata);

	for(int  i = 0; i < 4; i++) {
		std::cout << odata[i] << std::endl;
	}

	//return 0;

	Field3DOutputFile ofp;

	if(!ofp.create("test_dif.dif")) {
		std::cout << "Error opening output file" << std::endl;
		return -1;
	}

	dif.save(ofp);


	Field3DInputFile ifp;

	if(!ifp.open("test_dif.dif")) {
		std::cout << "Error opening input file" << std::endl;
		return -1;
	}

	DifImage<float> difi(V2i(0,0));
	if(!difi.load(ifp)) {
		std::cout << "Error loading deep image file" << std::endl;
		return -1;		
	}

	std::cout << "depth Levels: " << difi.depthLevels() << std::endl;
	std::cout << "number of channels: " << difi.numberOfChannels() << std::endl;

	float testdata[4];

//	difi.writeData(V2i(0,0), 0.0f, data);
//	difi.writeData(V2i(0,0), 23.1f, data);

	difi.readData(V2i(0,0), 23.0f, testdata, DifImage<float>::eLinear);
	for(int  i = 0; i < 4; i++) {
		std::cout << testdata[i] << " ";
	}

	std::cout << std::endl;

	float cret = 0.0f;
	difi.readChannelData("b", V2i(0,0), 23.1f, cret);

	std::cout << cret << std::endl;

	return 0;
}
