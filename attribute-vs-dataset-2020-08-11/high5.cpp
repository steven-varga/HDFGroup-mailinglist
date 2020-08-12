#include <vector>
#include <H5File.hpp>
#include <H5Group.hpp>
#include <H5DataSet.hpp>
#include <H5DataSpace.hpp>
#include <H5Attribute.hpp>
#include <armadillo>

using namespace HighFive;
int main(void) {
    File file("high5.h5", File::ReadWrite | File::Create | File::Truncate);
    Group group = file.createGroup("group");
    arma::wall_clock timer;
    size_t N = 8000;
    arma::vec data(N, arma::fill::randu);
    int I = 1;
    arma::vec datasetTime(I);
    std::vector<DataSet> datasetList;
    for (int i = 0; i < I; i++) {
        timer.tic();
        DataSet dataset = group.createDataSet<float>(std::to_string(i), DataSpace({ N }));
        datasetTime(i) = timer.toc();
        datasetList.push_back(dataset);
    }
    arma::vec attrTime(I);
    for (int i = 0; i < I; i++) {
        timer.tic();
        datasetList[i].createAttribute<float>("Attr", DataSpace({ N })).write((float*)data.memptr());
        attrTime(i) = timer.toc();
    }
    std::cout << arma::mean(datasetTime) << std::endl;
    std::cout << arma::mean(attrTime) << std::endl;
    file.flush();
}
