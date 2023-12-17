#include "MainMenuWidget.h"

#include "MenuHUD.h"

#define LOCTEXT_NAMESPACE "MainMenu"

void SMainMenuWidget::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	OwningHUD = InArgs._OwningHUD;


	const FMargin ContentPadding = FMargin(500.f, 300.f);
	const FMargin ButtonPadding = FMargin(10.f);


	const FText TitleText = LOCTEXT("GameTitle", "Aquarius Starter Game");
	const FText PlayText = LOCTEXT("PlayGame", "Play");
	const FText SettingsText = LOCTEXT("Settings", "Settings");
	const FText QuitText = LOCTEXT("QuitGame", "Quit Game");

	FSlateFontInfo ButtonTextStyle = FCoreStyle::Get().GetFontStyle("EmbossedText");
	ButtonTextStyle.Size = 40.f;

	FSlateFontInfo TitleTextStyle = ButtonTextStyle;
	TitleTextStyle.Size = 60.f;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.ColorAndOpacity(FColor::Black)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(ContentPadding)
		[
			SNew(SVerticalBox)

			//Title text
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Font(TitleTextStyle)
				.Text(TitleText)
				.Justification(ETextJustify::Center)
			]

			//Play button
			+ SVerticalBox::Slot()
			.Padding(ButtonPadding)
			[
				SNew(SButton)
				.OnClicked(this, &SMainMenuWidget::OnPlayClicked)
				[
					SNew(STextBlock)
					.Font(ButtonTextStyle)
					.Text(PlayText)
					.Justification(ETextJustify::Center)
				]
			]

			//Settings button
			+ SVerticalBox::Slot()
			.Padding(ButtonPadding)
			[
				SNew(SButton)
				[
					SNew(STextBlock)
					.Font(ButtonTextStyle)
					.Text(SettingsText)
					.Justification(ETextJustify::Center)
				]
			]

			//Quit button
			+ SVerticalBox::Slot()
			.Padding(ButtonPadding)
			[
				SNew(SButton)
				.OnClicked(this, &SMainMenuWidget::OnQuitClicked)
				[
					SNew(STextBlock)
					.Font(ButtonTextStyle)
					.Text(QuitText)
					.Justification(ETextJustify::Center)
				]
			]
		]
	];

}

FReply SMainMenuWidget::OnPlayClicked() const
{
	UE_LOG(LogTemp, Warning, TEXT("play clicked"));

	return FReply::Handled();
}

FReply SMainMenuWidget::OnQuitClicked() const
{
	if (OwningHUD.IsValid())
	{
		if (APlayerController* PC = OwningHUD->PlayerOwner)
		{
			PC->ConsoleCommand("quit");
		}
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
