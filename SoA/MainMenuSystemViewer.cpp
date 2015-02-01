#include "stdafx.h"
#include "MainMenuSystemViewer.h"

#include <Vorb/ui/InputDispatcher.h>
#include <Vorb/utils.h>

#include "Camera.h"
#include "SpaceSystem.h"
#include "SphericalTerrainPatch.h"

const float MainMenuSystemViewer::MIN_SELECTOR_SIZE = 12.0f;
const float MainMenuSystemViewer::MAX_SELECTOR_SIZE = 160.0f;

MainMenuSystemViewer::MainMenuSystemViewer(ui32v2 viewport, CinematicCamera* camera,
                                           SpaceSystem* spaceSystem, InputManager* inputManager) :
        m_viewport(viewport),
        m_camera(camera),
        m_spaceSystem(spaceSystem),
        m_inputManager(inputManager) {
    
    mouseButtons[0] = false;
    mouseButtons[1] = false;

    // Initialize the camera
    m_camera->setPosition(glm::dvec3(0.0, 200000.0, 0.0));

    // Initialize the camera
    m_camera->setClippingPlane(10000.0f, 3000000000000.0f);
    m_camera->setTarget(glm::dvec3(0.0, 0.0, 0.0), f32v3(1.0f, 0.0f, 0.0f), f32v3(0.0f, 0.0f, 1.0f), 20000.0);

    targetBody("Aldrin");

    // Register events
    m_hooks.addAutoHook(&vui::InputDispatcher::mouse.onButtonDown, [=](Sender s, const vui::MouseButtonEvent& e) { onMouseButtonDown(s, e); });
    m_hooks.addAutoHook(&vui::InputDispatcher::mouse.onButtonUp, [=](Sender s, const vui::MouseButtonEvent& e) { onMouseButtonUp(s, e); });
    m_hooks.addAutoHook(&vui::InputDispatcher::mouse.onMotion, [=](Sender s, const vui::MouseMotionEvent& e) { onMouseMotion(s, e); });
    m_hooks.addAutoHook(&vui::InputDispatcher::mouse.onWheel, [=](Sender s, const vui::MouseWheelEvent& e) { onMouseWheel(s, e); });
}

MainMenuSystemViewer::~MainMenuSystemViewer() {
    // Empty
}

void MainMenuSystemViewer::update() {
    const float HOVER_SPEED = 0.08f;
    const float HOVER_SIZE_INC = 7.0f;

    for (auto& it : m_spaceSystem->m_namePositionCT) {
        vcore::ComponentID componentID;

        f64v3 relativePos = it.second.position - m_camera->getPosition();
        f64 distance = glm::length(relativePos);
        float radiusPixels;
        float radius;

        BodyArData& data = bodyArData[it.first];
        float hoverTime = data.hoverTime;

        if (m_camera->pointInFrustum(f32v3(relativePos))) {
            data.inFrustum = true;
            // Get screen position 
            f32v3 screenCoords = m_camera->worldToScreenPoint(relativePos);
            f32v2 xyScreenCoords(screenCoords.x * m_viewport.x, screenCoords.y * m_viewport.y);

            // Get a smooth interpolator with hermite
            float interpolator = hermite(hoverTime);

            // See if it has a radius
            componentID = m_spaceSystem->m_sphericalGravityCT.getComponentID(it.first);
            if (componentID) {
                // Get radius of projected sphere
                radius = m_spaceSystem->m_sphericalGravityCT.get(componentID).radius;
                radiusPixels = (radius /
                                (tan(m_camera->getFieldOfView() / 2) * distance)) *
                                (m_viewport.y / 2.0f);
            } else {
                radius = 1000.0f;
                radiusPixels = (radius /
                                (tan(m_camera->getFieldOfView() / 2) * distance)) *
                                (m_viewport.y / 2.0f);
            }

            float selectorSize = radiusPixels * 2.0f + 3.0f;
            if (selectorSize < MIN_SELECTOR_SIZE) selectorSize = MIN_SELECTOR_SIZE;

            // Interpolate size
            selectorSize += interpolator * HOVER_SIZE_INC;

            // Detect mouse hover
            if (glm::length(m_mouseCoords - xyScreenCoords) <= selectorSize / 2.0f) {
                data.isHovering = true;
                data.hoverEntity = it.first;
                hoverTime += HOVER_SPEED;
                if (hoverTime > 1.0f) hoverTime = 1.0f;
            } else {
                data.isHovering = false;
                hoverTime -= HOVER_SPEED;
                if (hoverTime < 0.0f) hoverTime = 0.0f;
            }

            data.hoverTime = hoverTime;
            data.selectorSize = selectorSize;
        } else {
            data.isHovering = false;
            data.inFrustum = false;
        }
    }

    // Connect camera to target planet
    float length = m_camera->getFocalLength() / 10.0;
    if (length == 0) length = 0.1;
    m_camera->setClippingPlane(length, m_camera->getFarClip());
    // Target closest point on sphere
    m_camera->setTargetFocalPoint(getTargetPosition() -
                                 f64v3(glm::normalize(m_camera->getDirection())) * getTargetRadius());

}

void MainMenuSystemViewer::targetBody(const nString& name) {
    for (auto& it : m_spaceSystem->m_namePositionCT) {
        if (it.second.name == name) {
            targetBody(it.first);
            return;
        }
    }
}

void MainMenuSystemViewer::targetBody(vcore::EntityID eid) {
    m_targetEntity = eid;
    m_targetComponent = m_spaceSystem->m_namePositionCT.getComponentID(m_targetEntity);
}

f64v3 MainMenuSystemViewer::getTargetPosition() {
    return m_spaceSystem->m_namePositionCT.get(m_targetComponent).position;
}

f64 MainMenuSystemViewer::getTargetRadius() {
    return m_spaceSystem->m_sphericalGravityCT.get(m_targetComponent).radius;
}

nString MainMenuSystemViewer::getTargetName() {
    return m_spaceSystem->m_namePositionCT.get(m_targetComponent).name;
}

void MainMenuSystemViewer::onMouseButtonDown(Sender sender, const vui::MouseButtonEvent& e) {
    m_mouseCoords.x = e.x;
    m_mouseCoords.y = e.y;
    if (e.button == vui::MouseButton::LEFT) {
        mouseButtons[0] = true;
        // Target a body if we clicked on one
        f64 closestDist = 99999999999999999999999999999.0;
        vcore::EntityID closestEntity = 0;
        for (auto& it : bodyArData) {
            if (it.second.isHovering) {

                // Check distance so we pick only the closest one
                f64v3 pos = m_spaceSystem->m_namePositionCT.getFromEntity(it.first).position;
                f64 dist = glm::length(pos - m_camera->getPosition());
                if (dist < closestDist) {
                    closestDist = dist;
                    closestEntity = it.first;
                } else {
                    it.second.isLandSelected = false;
                }
            }
        }

        // If we selected an entity, then do the target picking
        if (closestEntity) {
            m_selectedPlanet = closestEntity;
            pickStartLocation(closestEntity);
            targetBody(closestEntity);
        }
    } else {
        mouseButtons[1] = true;
    }
}

void MainMenuSystemViewer::onMouseButtonUp(Sender sender, const vui::MouseButtonEvent& e) {
    m_mouseCoords.x = e.x;
    m_mouseCoords.y = e.y;
    if (e.button == vui::MouseButton::LEFT) {
        mouseButtons[0] = false;
    } else {
        mouseButtons[1] = false;
    }
}

void MainMenuSystemViewer::onMouseWheel(Sender sender, const vui::MouseWheelEvent& e) {
#define SCROLL_SPEED 0.1f
    m_camera->offsetTargetFocalLength(m_camera->getTargetFocalLength() * SCROLL_SPEED * -e.dy);
}

void MainMenuSystemViewer::onMouseMotion(Sender sender, const vui::MouseMotionEvent& e) {
    m_mouseCoords.x = e.x;
    m_mouseCoords.y = e.y;

#define MOUSE_SPEED 0.1f
    if (mouseButtons[0]) {
        m_camera->rotateFromMouse((float)-e.dx, (float)-e.dy, MOUSE_SPEED);
    }
    if (mouseButtons[1]) {
        m_camera->rollFromMouse((float)e.dx, MOUSE_SPEED);
    }
}

void MainMenuSystemViewer::pickStartLocation(vcore::EntityID eid) {
    f32v2 ndc = f32v2((m_mouseCoords.x / m_viewport.x) * 2.0f - 1.0f,
        1.0f - (m_mouseCoords.y / m_viewport.y) * 2.0f);
    f32v3 pickRay = m_camera->getPickRay(ndc);

    vcore::ComponentID cid = m_spaceSystem->m_namePositionCT.getComponentID(eid);
    if (!cid) return;
    f64v3 centerPos = m_spaceSystem->m_namePositionCT.get(cid).position;
    f32v3 pos = f32v3(centerPos - m_camera->getPosition());

    cid = m_spaceSystem->m_sphericalGravityCT.getComponentID(eid);
    if (!cid) return;
    float radius = m_spaceSystem->m_sphericalGravityCT.get(cid).radius;

    // Compute the intersection
    f32v3 normal, hitpoint;
    float distance;
    if (IntersectionUtils::sphereIntersect(pickRay, f32v3(0.0f), pos, radius, hitpoint, distance, normal)) {
        hitpoint -= pos;
        cid = m_spaceSystem->m_axisRotationCT.getComponentID(eid);
        if (cid) {
            f64q rot = m_spaceSystem->m_axisRotationCT.get(cid).currentOrientation;
            hitpoint = f32v3(glm::inverse(rot) * f64v3(hitpoint));
        }

        m_clickPos = f64v3(hitpoint);

        // Compute face and grid position
        computeGridPosition(hitpoint, radius);

        auto& data = bodyArData[eid];
        data.selectedPos = hitpoint;
        data.isLandSelected = true;
    }
}

// TODO(Ben): I think this isn't needed
void MainMenuSystemViewer::computeGridPosition(const f32v3& hitpoint, float radius) {
    f32v3 cornerPos[2];
    float min;
    f32v3 start = glm::normalize(hitpoint) * 2.0f * radius;
    f32v3 dir = -glm::normalize(hitpoint);
    cornerPos[0] = f32v3(-radius, -radius, -radius);
    cornerPos[1] = f32v3(radius, radius, radius);
    if (!IntersectionUtils::boxIntersect(cornerPos, dir,
        start, min)) {
        std::cerr << "Failed to find grid position from click\n";
    }

    f32v3 gridHit = start + dir * min;
    const float eps = 0.01f;

    if (abs(gridHit.x - (-radius)) < eps) { //-X
        m_selectedCubeFace = (int)WorldCubeFace::FACE_LEFT;
        m_selectedGridPos.x = gridHit.z;
        m_selectedGridPos.y = gridHit.y;
    } else if (abs(gridHit.x - radius) < eps) { //X
        m_selectedCubeFace = (int)WorldCubeFace::FACE_RIGHT;
        m_selectedGridPos.x = gridHit.z;
        m_selectedGridPos.y = gridHit.y;
    } else if (abs(gridHit.y - (-radius)) < eps) { //-Y
        m_selectedCubeFace = (int)WorldCubeFace::FACE_BOTTOM;
        m_selectedGridPos.x = gridHit.x;
        m_selectedGridPos.y = gridHit.z;
    } else if (abs(gridHit.y - radius) < eps) { //Y
        m_selectedCubeFace = (int)WorldCubeFace::FACE_TOP;
        m_selectedGridPos.x = gridHit.x;
        m_selectedGridPos.y = gridHit.z;
    } else if (abs(gridHit.z - (-radius)) < eps) { //-Z
        m_selectedCubeFace = (int)WorldCubeFace::FACE_BACK;
        m_selectedGridPos.x = gridHit.x;
        m_selectedGridPos.y = gridHit.y;
    } else if (abs(gridHit.z - radius) < eps) { //Z
        m_selectedCubeFace = (int)WorldCubeFace::FACE_FRONT;
        m_selectedGridPos.x = gridHit.x;
        m_selectedGridPos.y = gridHit.y;
    }
}