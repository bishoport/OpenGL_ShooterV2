#pragma once

#include "../LibCoreHeaders.h"
#include <filesystem>
#include "ModelLoader.h"
#include "AssetsManager.h"
#include "../StraightSkeleton/SkeletonBuilder.h"
#include <numeric>  // Para std::iota
#include <glm/glm.hpp>  // Para transformaciones de matrices
#include <glm/gtc/matrix_transform.hpp>

#include <mapbox/earcut.hpp>

struct Point2D {
    double x;
    double y;

    bool operator==(const Point2D& other) const {
        return x == other.x && y == other.y;
    }
};


// Especialización de mapbox::util::nth para Point2D
namespace mapbox {
    namespace util {
        template <>
        struct nth<0, Point2D> {
            inline static auto get(const Point2D& t) {
                return t.x;
            }
        };

        template <>
        struct nth<1, Point2D> {
            inline static auto get(const Point2D& t) {
                return t.y;
            }
        };
    } // namespace util
} // namespace mapbox

// Alias para los polígonos
using Polygon2D = std::vector<Point2D>;
using Polygons2D = std::vector<Polygon2D>;

namespace libCore {

    struct RoofData {
        std::vector<Vector2d> polygon;
        std::vector<Vector2d> hole;
        std::vector<std::vector<Vector2d>> holes;
        std::vector<Vector2d> expected;
    };

    class RoofGenerator {
    public:
        Skeleton BuildSkeleton(RoofData roofData);

        void AddRoofVerticesFromEdges(std::vector<GLfloat>& vertexBuffer, std::vector<GLuint>& indices, const Skeleton& skeleton, size_t maxEdges = std::numeric_limits<size_t>::max());
        void ApplyBoxUVProjection(std::vector<GLfloat>& vertexBuffer, float scale = 1.0f, float rotation = 0.0f);
        Ref<ModelContainer> CreateRoof(Skeleton sk, size_t maxEdges, float uvScale = 1.0f, float uvRotation = 0.0f);

    private:
        static bool EqualEpsilon(double d1, double d2);
        static bool ContainsEpsilon(const std::vector<Vector2d>& list, const Vector2d& p);
        static std::vector<Vector2d> GetFacePoints(const Skeleton& sk);
        static bool AssertExpectedPoints(const std::vector<Vector2d>& expectedList, const std::vector<Vector2d>& givenList);

        bool IsCounterClockwise(const Vector2d& p1, const Vector2d& p2, const Vector2d& p3);
        bool IsPointInTriangle(const Vector2d& pt, const Vector2d& v1, const Vector2d& v2, const Vector2d& v3);
        std::vector<size_t> TriangulatePolygon(const std::vector<Vector2d>& vertices);
    };
}
