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
    decltype(::GribSublimedDataInfoStruct::buf_pos_) buf_pos(100);
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
    GribSublimedDataInfoStruct data{.grid_data_=grid_def,.buf_pos_ = buf_pos,.sequence_time_ = TimeSequence(utc_tp(sys_days(1991y/1/1d)),system_clock::now(),
                        system_clock::duration((data.sequence_time_.get_interval().to()-data.sequence_time_.get_interval().from())/data.buf_pos_.size()))};

    {
        GribSublimedDataInfoStruct to_check;
        ASSERT_EQ(serialize<true>(data,buf),serialization::SerializationEC::NONE);
        ASSERT_EQ(deserialize<true>(to_check,std::span<const char>(buf)),serialization::SerializationEC::NONE);

        EXPECT_EQ(serial_size(to_check),serial_size(data));

        EXPECT_EQ(to_check.buf_pos_,data.buf_pos_);
        EXPECT_EQ(to_check.sequence_time_,data.sequence_time_);
        EXPECT_EQ(to_check.grid_data_,data.grid_data_);
    }
    {
        std::ofstream ofile("tmp",std::fstream::out|std::fstream::trunc);
        ASSERT_EQ(serialize_to_file(data,ofile),serialization::SerializationEC::NONE);
        ofile.close();
        GribSublimedDataInfoStruct to_check;
        std::ifstream ifile("tmp",std::fstream::in);
        ASSERT_EQ(deserialize_from_file(to_check,ifile),serialization::SerializationEC::NONE);
        ifile.close();
        std::filesystem::remove("tmp");
        EXPECT_EQ(serial_size(to_check),serial_size(data));

        EXPECT_EQ(to_check.buf_pos_,data.buf_pos_);
        EXPECT_EQ(to_check.sequence_time_,data.sequence_time_);
        EXPECT_EQ(to_check.grid_data_,data.grid_data_);
    }
}

TEST(Serialization, PathStorage){
    using namespace serialization;
    std::string path_tmp("a path to serialize");
    auto path_view = path::Storage<true>::file(path_tmp);
    std::vector<char> buf;
    ASSERT_EQ(serialize<true>(path_view,buf),SerializationEC::NONE);
    ASSERT_EQ(serial_size(path_view),serial_size(path_view.add_)+path_view.path_.size()+sizeof(path_view.type_));
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
    decltype(GribSublimedDataInfoStruct::buf_pos_) buf_pos(100);
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
            auto& vector_seq = data[path::Storage<false>::file(str)][std::make_shared<Grib1CommonDataProperties>(Organization::ECMWF,128,TimeFrame::HOUR,i+j*2+5*2)];
            for(int m=0;m<10;++m){
                GribSublimedDataInfoStruct sub_data{.grid_data_=grid_def,.buf_pos_ = buf_pos,.sequence_time_ = TimeSequence(utc_tp(sys_days(1991y/1/1d)),system_clock::now(),
                        system_clock::duration((sub_data.sequence_time_.get_interval().to()-sub_data.sequence_time_.get_interval().from())/sub_data.buf_pos_.size()))};
                vector_seq.push_back(std::move(sub_data));
            }
        }
    }
    std::vector<char> buf;
    ASSERT_EQ(serialize<true>(data,buf),SerializationEC::NONE);
    EXPECT_EQ(serial_size(data),buf.size());
    decltype(data) to_check;
    ASSERT_EQ(deserialize<true>(to_check,std::span<const char>(buf)),SerializationEC::NONE);
    EXPECT_EQ(to_check.size(),data.size());
    for(auto& [filename,file_data]:to_check){
        ASSERT_TRUE(data.contains(filename));
        for(auto& [cmn,data_info]:file_data){
            ASSERT_TRUE(data[filename].contains(cmn));
            ASSERT_EQ(data_info,data[filename][cmn]);
        }
    }
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