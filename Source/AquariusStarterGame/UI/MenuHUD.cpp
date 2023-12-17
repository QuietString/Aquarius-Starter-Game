#include "MenuHUD.h"
#include "MainMenuWidget.h"
#include "Widgets/SWeakWidget.h"

void AMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	//ShowMenu();
}

void AMenuHUD::ShowMenu()
{
	check(GEngine && GEngine->GameViewport)

	MenuWidget = SNew(SMainMenuWidget).OwningHUD(this);
	GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(MenuWidgetContainer, SWeakWidget).PossiblyNullContent(MenuWidget.ToSharedRef()));

	if (PlayerOwner)
	{
		PlayerOwner->bShowMouseCursor = true;
		PlayerOwner->SetInputMode(FInputModeUIOnly());
	}
}

void AMenuHUD::RemoveMenu()
{
	if (GEngine && GEngine->GameViewport && MenuWidget.IsValid() && MenuWidgetContainer.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(MenuWidgetContainer.ToSharedRef());

		if (PlayerOwner)
		{
			PlayerOwner->bShowMouseCursor = false;
			PlayerOwner->SetInputMode(FInputModeGameOnly());
		}
	}
}
