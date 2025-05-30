/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
/*
 * WARNING: DO NOT EDIT THIS FILE!
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE SIMPLEITK BUILD PROCESS.
 * Please look at sitkImageFilterTestTemplate.cxx.in to make changes.
 */

#include <SimpleITKTestHarness.h>

#include <sitkTransformGeometryImageFilter.h>
#include <sitkImageFileReader.h>
#include <sitkImageFileWriter.h>
#include <sitkHashImageFilter.h>
#include <sitkCastImageFilter.h>
#include <sitkConditional.h>
#include <sitkVersion.h>

#include <array>


static bool
isLabelType(const itk::simple::Image & img)
{
  static const std::array<itk::simple::PixelIDValueEnum, 4> labelIDs{ { itk::simple::sitkLabelUInt8,
                                                                        itk::simple::sitkLabelUInt16,
                                                                        itk::simple::sitkLabelUInt32,
                                                                        itk::simple::sitkLabelUInt64 } };

  return std::find(labelIDs.begin(), labelIDs.end(), img.GetPixelID()) != labelIDs.end();
}

TEST(BasicFilters, TransformGeometryImageFilter)
{
  itk::simple::ImageFileReader              reader;
  itk::simple::TransformGeometryImageFilter filter;
  itk::simple::Image                        inputA(0, 0, itk::simple::sitkUInt8);
  itk::simple::Image                        inputB(0, 0, itk::simple::sitkUInt8);
  itk::simple::Image                        output(0, 0, itk::simple::sitkUInt8);
  std::string                               inputSHA1hash;


  EXPECT_NE(filter.ToString(), "") << "empty ToString method!";
  EXPECT_EQ(filter.GetName(), "TransformGeometryImageFilter") << "checking GetName method!";

  // Test sets / gets


  // This block is to verify that the input images for named inputs are
  // check if the sizes match for computation.
  itk::simple::Image smallImage(1, 1, 1, itk::simple::sitkUInt8);
  itk::simple::Image largeImage(10, 10, 10, itk::simple::sitkUInt8);
}

TEST(BasicFilters, TransformGeometryImageFilter_defaults)
{ // BEGIN FOR EACH TEST
  /* TAG: defaults DESCRIPTION: run with some defaults */

  itk::simple::ImageFileReader              reader;
  itk::simple::TransformGeometryImageFilter filter;
  itk::simple::Image                        output;
  itk::simple::HashImageFilter              hasher;
  std::string                               inputSHA1hash;

  std::vector<std::string> inputFileNames;

  inputFileNames.push_back("Input/RA-Float.nrrd");
  inputFileNames.push_back("Input/xforms/affine_i_3.txt");

  itk::simple::Image     input1 = itk::simple::ReadImage(dataFinder.GetFile(inputFileNames[0]));
  itk::simple::Transform input2 = itk::simple::ReadTransform(dataFinder.GetFile(inputFileNames[1]));

  inputSHA1hash = itk::simple::Hash(input1);

  // Do we get the same image back, if we use the functional interface?
  itk::simple::Image fromFunctional(0, 0, itk::simple::sitkUInt8);
  itk::simple::Image fromProcedural(0, 0, itk::simple::sitkUInt8);
  EXPECT_NO_THROW(fromProcedural = filter.Execute(input1, input2))
    << "Procedural interface to TransformGeometryImageFilter";
  EXPECT_NO_THROW(fromFunctional = itk::simple::TransformGeometry(input1, input2))
    << "Functional interface to TransformGeometryImageFilter";
  hasher.SetHashFunction(itk::simple::HashImageFilter::MD5);
  EXPECT_EQ(hasher.Execute(fromProcedural), hasher.Execute(fromFunctional))
    << "procedural and functional are not the same!";

  filter.DebugOn();
  ASSERT_NO_THROW(output = filter.Execute(input1, input2));

  // Check that the input is not modified
  // This will fail if the filter is being run in-place
  EXPECT_EQ(inputSHA1hash, itk::simple::Hash(input1)) << "Input was modified by filter.";

  // Check that the input buffer is different than the output
  // buffer, this does not check complex or label map pixel types.
  if (!isLabelType(output) && !isLabelType(input1))
  {
    EXPECT_NE(output.GetBufferAsVoid(), input1.GetBufferAsVoid()) << "Input buffer was copyied to output!";
  }

  itk::simple::Image inPlaceOutput;
  itk::simple::Image temp = input1;
  temp.MakeUnique();
  ASSERT_NO_THROW(inPlaceOutput = filter.Execute(std::move(temp), input2));

  ASSERT_NE(nullptr, inPlaceOutput.GetITKBase());
  EXPECT_EQ(itk::simple::Hash(output), itk::simple::Hash(inPlaceOutput)) << "InPlace value does not match!";

  ASSERT_NE(nullptr, input1.GetITKBase());
  if (!isLabelType(input1))
  {
    EXPECT_NE(nullptr, input1.GetBufferAsVoid()) << "An input is missing it's buffer!";

    if (!isLabelType(inPlaceOutput))
    {
      EXPECT_NE(inPlaceOutput.GetBufferAsVoid(), input1.GetBufferAsVoid()) << "An input and output buffer matches.";
    }
  }

  IMAGECOMPAREWITHTOLERANCE(output, "", 0.001);

} // END FOR EACH TEST
