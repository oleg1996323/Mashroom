#include "data/sublimed_info.h"
#include "data/info.h"
#include "definitions/path_process.h"
#include <gtest/gtest.h>
#include <numeric>
#include "../utility/random_char.h"

TEST(Serialization, SublimedInfo){
    using namespace serialization;
    using namespace std::chrono;
    std::vector<char> buf;
    decltype(SublimedDataInfo::buf_pos_) buf_pos(100);
    std::ranges::iota(buf_pos,0);
    GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid_def;
    grid_def.base_.x1=18;
    grid_def.base_.x2=68;
    grid_def.base_.y1=20;
    grid_def.base_.y2=70;
    grid_def.base_.nx=50;
    grid_def.base_.ny=50;
    grid_def.base_.dx=1;
    grid_def.base_.dy=1;
    ResolutionComponentFlags flags;
    flags.earth_spheroidal = false;
    flags.given_direction = true;
    flags.grid_direction_uv_comp = false;
    grid_def.base_.resolutionAndComponentFlags=flags;
    ScanMode scan;
    scan.adj_points_j_dir = true;
    scan.points_sub_i_dir = true;
    scan.points_sub_j_dir = false;
    grid_def.base_.scan_mode = scan;
    SublimedDataInfo data{.grid_data_=grid_def,.buf_pos_ = buf_pos,.from_=utc_tp(sys_days(1991y/1/1d)),.to_=system_clock::now(),
                        .discret_=system_clock::duration((data.to_-data.from_)/data.buf_pos_.size())};

    {
        SublimedDataInfo to_check;
        ASSERT_EQ(serialize<true>(data,buf),serialization::SerializationEC::NONE);
        ASSERT_EQ(deserialize<true>(to_check,std::span<const char>(buf)),serialization::SerializationEC::NONE);

        EXPECT_EQ(serial_size(to_check),serial_size(data));

        EXPECT_EQ(to_check.buf_pos_,data.buf_pos_);
        EXPECT_EQ(to_check.discret_,data.discret_);
        EXPECT_EQ(to_check.from_,data.from_);
        EXPECT_EQ(to_check.to_,data.to_);
        EXPECT_EQ(to_check.grid_data_,data.grid_data_);
    }
    {
        std::ofstream ofile("tmp",std::fstream::out|std::fstream::trunc);
        ASSERT_EQ(serialize_to_file(data,ofile),serialization::SerializationEC::NONE);
        ofile.close();
        SublimedDataInfo to_check;
        std::ifstream ifile("tmp",std::fstream::in);
        ASSERT_EQ(deserialize_from_file(to_check,ifile),serialization::SerializationEC::NONE);
        ifile.close();
        std::filesystem::remove("tmp");
        EXPECT_EQ(serial_size(to_check),serial_size(data));

        EXPECT_EQ(to_check.buf_pos_,data.buf_pos_);
        EXPECT_EQ(to_check.discret_,data.discret_);
        EXPECT_EQ(to_check.from_,data.from_);
        EXPECT_EQ(to_check.to_,data.to_);
        EXPECT_EQ(to_check.grid_data_,data.grid_data_);
    }
}

TEST(Serialization, PathStorage){
    using namespace serialization;
    std::string path_tmp("a path to serialize");
    path::Storage<true> path_view(path_tmp,path::TYPE::FILE);
    std::vector<char> buf;
    ASSERT_EQ(serialize<true>(path_view,buf),SerializationEC::NONE);
    ASSERT_EQ(serial_size(path_view),sizeof(size_t)+path_view.path_.size()+sizeof(path_view.type_));
    {
        path::Storage<false> path;
        ASSERT_EQ(deserialize<true>(path,std::span<const char>(buf)),SerializationEC::NONE);
        EXPECT_EQ(path,path_view);
    }
    {
        std::ofstream ofile("tmp",std::fstream::out|std::fstream::trunc);
        ASSERT_EQ(serialize_to_file(path_view,ofile),serialization::SerializationEC::NONE);
        ofile.close();
        path::Storage<false> path;
        std::ifstream ifile("tmp",std::fstream::in);
        ASSERT_EQ(deserialize_from_file(path,ifile),serialization::SerializationEC::NONE);
        ifile.close();
        std::filesystem::remove("tmp");
        EXPECT_EQ(path,path_view);
    }
}

TEST(Serialization,SublimedGribDataInfo){
    using namespace serialization;
    decltype(SublimedDataInfo::buf_pos_) buf_pos(100);
    std::ranges::iota(buf_pos,0);
    GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid_def;
    grid_def.base_.x1=18;
    grid_def.base_.x2=68;
    grid_def.base_.y1=20;
    grid_def.base_.y2=70;
    grid_def.base_.nx=50;
    grid_def.base_.ny=50;
    grid_def.base_.dx=1;
    grid_def.base_.dy=1;
    ResolutionComponentFlags flags;
    flags.earth_spheroidal = false;
    flags.given_direction = true;
    flags.grid_direction_uv_comp = false;
    grid_def.base_.resolutionAndComponentFlags=flags;
    ScanMode scan;
    scan.adj_points_j_dir = true;
    scan.points_sub_i_dir = true;
    scan.points_sub_j_dir = false;
    grid_def.base_.scan_mode = scan;
    SublimedGribDataInfo::sublimed_data_t data;
    for(int i=0;i<10;++i){
        std::string str;
        str.resize(10);
        std::generate(str.begin(),str.end(),getRandomChar);
        for(int j=0;j<10;++j){
            auto& vector_seq = data[path::Storage<false>(str,path::TYPE::FILE)][std::make_shared<CommonDataProperties>(Organization::ECMWF,128,TimeFrame::HOUR,i+j*2+5*2)];
            for(int m=0;m<10;++m){
                SublimedDataInfo sub_data{.grid_data_=grid_def,.buf_pos_ = buf_pos,.from_=utc_tp(sys_days(1991y/1/(i+j+m))),.to_=system_clock::now(),
                        .discret_=system_clock::duration((sub_data.to_-sub_data.from_)/sub_data.buf_pos_.size())};
                vector_seq.push_back(std::move(sub_data));
            }
        }
    }
    std::vector<char> buf;
    ASSERT_EQ(serialize<true>(data,buf),SerializationEC::NONE);
    EXPECT_EQ(serial_size(data),buf.size());
    decltype(data) to_check;
    ASSERT_EQ(deserialize<true>(to_check,std::span<const char>(buf)),SerializationEC::NONE);
    EXPECT_EQ(to_check,data);
}


int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}

static_assert(serialization::min_serial_size<double>()==sizeof(double));
static_assert(serialization::max_serial_size<double>()==sizeof(double));
static_assert(serialization::min_serial_size<std::chrono::system_clock::time_point>()==sizeof(std::chrono::system_clock::time_point));
static_assert(serialization::max_serial_size<std::chrono::system_clock::time_point>()==sizeof(std::chrono::system_clock::time_point));
static_assert(serialization::min_serial_size<std::chrono::system_clock::duration>()==sizeof(std::chrono::system_clock::duration));
static_assert(serialization::max_serial_size<std::chrono::system_clock::duration>()==sizeof(std::chrono::system_clock::duration));
static_assert(serialization::min_serial_size<std::shared_ptr<int>>()==sizeof(bool));
static_assert(serialization::max_serial_size<std::shared_ptr<int>>()==sizeof(bool)+sizeof(int));
static_assert(serialization::min_serial_size<std::chrono::years>()==sizeof(std::chrono::system_clock::duration));
static_assert(serialization::max_serial_size<std::chrono::system_clock::duration>()==sizeof(std::chrono::system_clock::duration));
static_assert(serialization::min_serial_size<std::vector<ptrdiff_t>>()==sizeof(size_t));
static_assert(serialization::max_serial_size<std::vector<ptrdiff_t>>()==std::numeric_limits<size_t>::max());