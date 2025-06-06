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
#ifndef sitkScaleTransform_h
#define sitkScaleTransform_h

#include "sitkCommon.h"
#include "sitkTransform.h"

namespace itk::simple
{

/** \class ScaleTransform
 * \brief A 2D or 3D anisotropic scale of coordinate space around
 * a fixed center.
 *
 * \sa itk::ScaleTransform
 */
class SITKCommon_EXPORT ScaleTransform : public Transform
{
public:
  using Self = ScaleTransform;
  using Superclass = Transform;

  ~ScaleTransform() override;

  explicit ScaleTransform(unsigned int dimensions, const std::vector<double> & scale = std::vector<double>(3, 1.0));

  ScaleTransform(const ScaleTransform &);

  explicit ScaleTransform(const Transform &);

  /** Name of this class */
  std::string
  GetName() const override
  {
    return std::string("ScaleTransform");
  }

  ScaleTransform &
  operator=(const ScaleTransform &);


  void
  SetScale(const std::vector<double> & params);
  std::vector<double>
  GetScale() const;

  /** fixed parameter */
  void
  SetCenter(const std::vector<double> & params);
  std::vector<double>
  GetCenter() const;

  /** additional methods */
  std::vector<double>
  GetMatrix() const;

protected:
  void
  SetPimpleTransform(std::unique_ptr<PimpleTransformBase> && pimpleTransform) override;

private:
  struct MyVisitor
  {
    itk::TransformBase * transform;
    ScaleTransform *     that;
    template <typename TransformType>
    void
    operator()() const
    {
      TransformType * t = dynamic_cast<TransformType *>(transform);
      if (t && (typeid(*t) == typeid(TransformType)))
      {
        that->InternalInitialization<TransformType>(t);
      }
    }
  };

  void
  InternalInitialization(itk::TransformBase * transform);

  template <typename TransformType>
  void
  InternalInitialization(TransformType * transform);


  std::function<void(std::vector<double>)> m_pfSetCenter;
  std::function<std::vector<double>()>     m_pfGetCenter;
  std::function<void(std::vector<double>)> m_pfSetScale;
  std::function<std::vector<double>()>     m_pfGetScale;
  std::function<std::vector<double>()>     m_pfGetMatrix;
};

} // namespace itk::simple

#endif // sitkScaleTransform_h
