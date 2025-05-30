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
#ifndef sitkScaleVersor3DTransform_h
#define sitkScaleVersor3DTransform_h

#include "sitkCommon.h"
#include "sitkTransform.h"

namespace itk::simple
{


/** \class ScaleVersor3DTransform
 * \brief A parameterized 3D transform composed of the
 * addition of a versor rotation matrix and a scale matrix around a
 * fixed center with translation.
 *
 * \sa itk::ScaleVersor3DTransform
 */
class SITKCommon_EXPORT ScaleVersor3DTransform : public Transform
{
public:
  using Self = ScaleVersor3DTransform;
  using Superclass = Transform;

  ~ScaleVersor3DTransform() override;

  // construct identity
  ScaleVersor3DTransform();

  ScaleVersor3DTransform(const ScaleVersor3DTransform &);

  explicit ScaleVersor3DTransform(const Transform &);

  ScaleVersor3DTransform(const std::vector<double> & scale,
                         const std::vector<double> & versor,
                         const std::vector<double> & translation = std::vector<double>(3, 0.0),
                         const std::vector<double> & fixedCenter = std::vector<double>(3, 0.0));

  ScaleVersor3DTransform(const std::vector<double> & scale,
                         const std::vector<double> & axis,
                         double                      angle,
                         const std::vector<double> & translation = std::vector<double>(3, 0.0),
                         const std::vector<double> & fixedCenter = std::vector<double>(3, 0.0));

  ScaleVersor3DTransform &
  operator=(const ScaleVersor3DTransform &);

  /** Name of this class */
  std::string
  GetName() const override
  {
    return std::string("ScaleVersor3DTransform");
  }

  /** fixed parameter */
  void
  SetCenter(const std::vector<double> & params);
  std::vector<double>
  GetCenter() const;


  /** parameter */
  void
  SetRotation(const std::vector<double> & versor);
  void
  SetRotation(const std::vector<double> & axis, double angle);
  std::vector<double>
  GetVersor() const;

  std::vector<double>
  GetTranslation() const;
  void
  SetTranslation(const std::vector<double> & translation);

  std::vector<double>
  GetScale() const;
  void
  SetScale(const std::vector<double> & scale);

  /** additional methods */
  void
  Translate(const std::vector<double> & offset);
  std::vector<double>
  GetMatrix() const;

protected:
  void
  SetPimpleTransform(std::unique_ptr<PimpleTransformBase> && pimpleTransform) override;

private:
  void
  InternalInitialization(itk::TransformBase * transform);

  template <typename TransformType>
  void
  InternalInitialization(TransformType * transform);


  std::function<void(const std::vector<double> &)>         m_pfSetCenter;
  std::function<std::vector<double>()>                     m_pfGetCenter;
  std::function<void(const std::vector<double> &)>         m_pfSetRotation1;
  std::function<void(const std::vector<double> &, double)> m_pfSetRotation2;
  std::function<std::vector<double>()>                     m_pfGetVersor;
  std::function<void(const std::vector<double> &)>         m_pfSetTranslation;
  std::function<std::vector<double>()>                     m_pfGetTranslation;
  std::function<void(const std::vector<double> &)>         m_pfSetScale;
  std::function<std::vector<double>()>                     m_pfGetScale;
  std::function<void(const std::vector<double> &)>         m_pfTranslate;
  std::function<std::vector<double>()>                     m_pfGetMatrix;
};

} // namespace itk::simple

#endif // sitkVersorTransform_h
