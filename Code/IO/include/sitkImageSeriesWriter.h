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
#ifndef sitkImageSeriesWriter_h
#define sitkImageSeriesWriter_h

#include "sitkMacro.h"
#include "sitkImage.h"
#include "sitkProcessObject.h"
#include "sitkPathType.h"
#include "sitkIO.h"
#include "sitkMemberFunctionFactory.h"

namespace itk
{

// Forward declaration for pointer
class ImageIOBase;

namespace simple
{

/** \class ImageSeriesWriter
 * \brief Writer series of image from a SimpleITK image.
 *
 * The ImageSeriesWriter is for writing a 3D image as a series of
 * 2D images. A list of names for the series of 2D images must be
 * provided, and an exception will be generated if the number of
 * file names does not match the size of the image in the z-direction.
 *
 * DICOM series cannot be written with this class, as an exception
 * will be generated. To write a DICOM series the individual
 * slices must be extracted, proper DICOM tags must be added to
 * the dictionaries, then written with the ImageFileWriter.
 *
 * \sa itk::simple::WriteImage for the procedural interface
 **/
class SITKIO_EXPORT ImageSeriesWriter : public ProcessObject
{
public:
  using Self = ImageSeriesWriter;

  ~ImageSeriesWriter() override;

  ImageSeriesWriter();

  /** Print ourselves to string */
  std::string
  ToString() const override;

  /** \brief Get a vector of the names of registered itk ImageIOs
   */
  virtual std::vector<std::string>
  GetRegisteredImageIOs() const;

  /** \brief Set/Get name of ImageIO to use
   *
   * An option to override the automatically detected ImageIO used
   * to write the image. The available ImageIOs are listed by the
   * GetRegisteredImageIOs method. If the ImageIO can not be
   * constructed an exception will be generated.
   *
   * The  default value is an empty string (""). This indicates
   * that the ImageIO will be automatically determined by the ITK
   * ImageIO factory mechanism.
   * @{
   */
  virtual void
  SetImageIO(const std::string & imageio);
  virtual std::string
  GetImageIO() const;
  /* @} */

  /** return user readable name of the filter */
  std::string
  GetName() const override
  {
    return std::string("ImageSeriesWriter");
  }

  /** \brief Enable compression if available for file type.
   *
   * These methods Set/Get/Toggle the UseCompression flag which
   * gets passed to image file's itk::ImageIO object. This is
   * only a request as not all file formats support compression.
   * @{ */
  void
  SetUseCompression(bool UseCompression);
  bool
  GetUseCompression() const;

  void
  UseCompressionOn()
  {
    return this->SetUseCompression(true);
  }
  void
  UseCompressionOff()
  {
    return this->SetUseCompression(false);
  }
  /** @} */

  /** \brief A hint for the amount of compression to be applied during writing.
   *
   *  After compression is enabled and if the itk::ImageIO support this option, then this value may be used. The range
   * is dependent upon the compression algorithm used. It is generally 0-100 for JPEG like lossy compression and 0-9 for
   * lossless zip or LZW like compression algorithms.  Please see the specific itk::ImageIO for details.
   * @{ */
  void
  SetCompressionLevel(int);
  int
  GetCompressionLevel() const;
  /** @} */

  /** \brief A compression algorithm hint
   *
   * The default is an empty string which enables the default compression of the ImageIO if compression is enabled.
   * If the string identifier is not known a warning is produced and the default compressor is used. Please see the
   * itk::ImageIO for details.
   * @{ */
  void
  SetCompressor(const std::string &);
  std::string
  GetCompressor();
  /** @} */

  /** The filenames to where the image slices are written.
   *
   * The number of filenames must match the number of slices in
   * the image.
   * @{ */
  void
  SetFileNames(const std::vector<PathType> & fileNames);
  const std::vector<PathType> &
  GetFileNames() const;
  /** @} */


  void
  Execute(const Image &);
  void
  Execute(const Image & image, const std::vector<PathType> & inFileNames, bool useCompression, int compressionLevel);

protected:
  template <class TImageType>
  void
  ExecuteInternal(const Image & inImage);

private:
  itk::SmartPointer<ImageIOBase>
  GetImageIOBase(const PathType & fileName);

  // function pointer type
  typedef void (Self::*MemberFunctionType)(const Image &);

  // friend to get access to executeInternal member
  friend struct detail::MemberFunctionAddressor<MemberFunctionType>;
  static const detail::MemberFunctionFactory<MemberFunctionType> &
  GetMemberFunctionFactory();

  bool        m_UseCompression{ false };
  int         m_CompressionLevel{ -1 };
  std::string m_Compressor;

  std::vector<PathType> m_FileNames;

  std::string m_ImageIOName;
};


/**
 * \brief WriteImage is a procedural interface to the ImageSeriesWriter.
 *     class which is convenient for many image writing tasks.
 *
 *  \param image the input image to be written
 *  \param fileNames a vector of filenames of length equal to the
 *  number of slices in the image.
 *  \param useCompression request to compress the written file
 *  \param compressionLevel a hint for the amount of compression to
 *    be applied during writing.
 *
 *
 * \sa itk::simple::ImageFileWriter for writing a single file.
 */
SITKIO_EXPORT void
WriteImage(const Image &                 image,
           const std::vector<PathType> & fileNames,
           bool                          useCompression = false,
           int                           compressionLevel = -1);
} // namespace simple
} // namespace itk

#endif
