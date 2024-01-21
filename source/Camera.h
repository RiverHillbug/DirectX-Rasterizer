#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		}

		Vector3 origin{};
		float fovAngle{ 45.0f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.0f) };
		float aspectRatio{};
		float nearPlane{ 0.1f };
		float farPlane{ 1000.0f };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};
		const float cameraMovementSpeed{ 10.0f };
		const float cameraRotationSpeed{ 2.0f };

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix projectionMatrix{};

		void Initialize(float _fovAngle = 90.0f, Vector3 _origin = { 0.0f, 0.0f, 0.0f }, float aspect = 1.0f)
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.0f);
			origin = _origin;
			aspectRatio = aspect;

			CalculateProjectionMatrix();
		}

		void CalculateViewMatrix()
		{
			invViewMatrix = Matrix::CreateRotation(totalPitch, totalYaw, 0.0f);
			invViewMatrix *= Matrix::CreateTranslation(origin);

			//Inverse(ONB) => ViewMatrix
			viewMatrix = Matrix::Inverse(invViewMatrix);

			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
		}

		void CalculateProjectionMatrix()
		{
			projectionMatrix = Matrix::CreatePerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);

			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(const Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			Vector3 movementDirection{ };

			if (pKeyboardState[SDL_SCANCODE_W])
			{
				++movementDirection.z;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				--movementDirection.x;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				--movementDirection.z;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				++movementDirection.x;
			}

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if (mouseState & SDL_BUTTON_RMASK && mouseState & SDL_BUTTON_LMASK)
			{
				movementDirection.y -= mouseY;
			}
			else if (mouseState & SDL_BUTTON_RMASK)
			{
				totalYaw += mouseX * cameraRotationSpeed * deltaTime;
				totalPitch -= mouseY * cameraRotationSpeed * deltaTime;
			}
			else if (mouseState & SDL_BUTTON_LMASK)
			{
				totalYaw += mouseX * cameraRotationSpeed * deltaTime;
				movementDirection.z -= mouseY;
			}

			if (movementDirection != Vector3::Zero)
			{
				movementDirection = (movementDirection.x * invViewMatrix.GetAxisX()) + (movementDirection.y * invViewMatrix.GetAxisY()) + (movementDirection.z * invViewMatrix.GetAxisZ());

				origin += movementDirection.Normalized() * cameraMovementSpeed * deltaTime;
			}

			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}
	};
}
