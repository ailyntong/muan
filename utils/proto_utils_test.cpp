#include "proto_utils.h"
#include "utils/test_proto.pb.h"
#include "gtest/gtest.h"
#include <iostream>

TEST(ProtoUtils, ProtoToCSV) {
  TestProto p;
  p.set_test_string("Citrus Circuits");
  p.set_test_uint(1678);
  p.mutable_sub_message()->set_id(100);
  p.mutable_sub_message()->set_num(3.14159);
  p.set_is_sane(false);
  ASSERT_EQ(muan::util::ProtoToCSVHeader(p),
            "test_string,test_uint,id,num,is_sane");
  ASSERT_EQ(muan::util::ProtoToCSV(p), "Citrus Circuits,1678,100,3.141590,0");
}
