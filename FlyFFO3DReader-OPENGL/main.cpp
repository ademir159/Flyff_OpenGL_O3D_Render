
#include "LoadedObject.hpp"

int main() {
	//LoadedObject obj = LoadedObject();// );

	LoadedObject* pObject3D = new LoadedObject;

	if (pObject3D->LoadObject("E:\\Flyff\\FlyFFO3DReader-master\\Release\\Part_mMer03Upper.o3d") == 0)
	{
		return 0;
	}
	while (true)
	{

	}

	//    std::vector<float> vertices = obj.getVerticeList();
	//    std::vector<int> indices = obj.getIndiceList();
	  //  std::vector< pair < float, float > > a = obj.getUvs();
	   // cout << obj.getTextureName() << endl;
	return 1;
}


