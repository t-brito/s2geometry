// Copyright 2013 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// Author: ericv@google.com (Eric Veach)

#include "s2/s2lax_polyline_shape.h"

#include <utility>

#include <gtest/gtest.h>
#include "s2/s2shapeutil_testing.h"
#include "s2/s2text_format.h"

using std::vector;

TEST(S2LaxPolylineShape, NoVertices) {
  vector<S2Point> vertices;
  S2LaxPolylineShape shape(vertices);
  EXPECT_EQ(0, shape.num_edges());
  EXPECT_EQ(0, shape.num_chains());
  EXPECT_EQ(1, shape.dimension());
  EXPECT_TRUE(shape.is_empty());
  EXPECT_FALSE(shape.is_full());
  EXPECT_FALSE(shape.GetReferencePoint().contained);
}

TEST(S2LaxPolylineShape, OneVertex) {
  vector<S2Point> vertices = {S2Point(1, 0, 0)};
  S2LaxPolylineShape shape(vertices);
  EXPECT_EQ(0, shape.num_edges());
  EXPECT_EQ(0, shape.num_chains());
  EXPECT_EQ(1, shape.dimension());
  EXPECT_TRUE(shape.is_empty());
  EXPECT_FALSE(shape.is_full());
}

TEST(S2LaxPolylineShape, MoveConstructor) {
  std::unique_ptr<S2LaxPolylineShape> original =
      s2textformat::MakeLaxPolylineOrDie("1:1, 4:4");
  S2LaxPolylineShape moved(std::move(*original));
  ASSERT_EQ(0, original->num_vertices());
  ASSERT_EQ(2, moved.num_vertices());
}

TEST(S2LaxPolylineShape, MoveAssignmentOperator) {
  std::unique_ptr<S2LaxPolylineShape> original =
      s2textformat::MakeLaxPolylineOrDie("1:1, 4:4");
  S2LaxPolylineShape moved;
  moved = std::move(*original);
  ASSERT_EQ(0, original->num_vertices());
  ASSERT_EQ(2, moved.num_vertices());
}

TEST(S2LaxPolylineShape, EdgeAccess) {
  vector<S2Point> vertices = s2textformat::ParsePointsOrDie("0:0, 0:1, 1:1");
  S2LaxPolylineShape shape(vertices);
  EXPECT_EQ(2, shape.num_edges());
  EXPECT_EQ(1, shape.num_chains());
  EXPECT_EQ(0, shape.chain(0).start);
  EXPECT_EQ(2, shape.chain(0).length);
  EXPECT_EQ(1, shape.dimension());
  EXPECT_FALSE(shape.is_empty());
  EXPECT_FALSE(shape.is_full());
  auto edge0 = shape.edge(0);
  EXPECT_EQ(vertices[0], edge0.v0);
  EXPECT_EQ(vertices[1], edge0.v1);
  auto edge1 = shape.edge(1);
  EXPECT_EQ(vertices[1], edge1.v0);
  EXPECT_EQ(vertices[2], edge1.v1);
}

TEST(EncodedS2LaxPolylineShape, RoundtripEncoding) {
  vector<S2Point> vertices = s2textformat::ParsePointsOrDie("0:0, 0:1, 1:1");
  S2LaxPolylineShape shape(vertices);

  Encoder encoder;
  shape.Encode(&encoder, s2coding::CodingHint::COMPACT);
  Decoder a_decoder(encoder.base(), encoder.length());
  EncodedS2LaxPolylineShape a_shape;
  a_shape.Init(&a_decoder);

  Encoder b_encoder;
  a_shape.Encode(&b_encoder, s2coding::CodingHint::COMPACT);
  Decoder b_decoder(b_encoder.base(), b_encoder.length());
  EncodedS2LaxPolylineShape b_shape;
  b_shape.Init(&b_decoder);
  s2testing::ExpectEqual(shape, b_shape);
}
