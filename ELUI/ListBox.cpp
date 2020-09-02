#include "ListBox.hpp"

void UIListBox::_UpdateLabels()
{
	size_t i = 1;
	for (UILabel& label : _labels)
	{
		label.SetBounds(0.f, UICoord(1.f, i * -_itemHeight), 1.f, UICoord(0.f, _itemHeight));
		label.SetFont(_font);
		label.SetColour(_textColour);
		label.SetShadowColour(_textShadowColour);
		label.SetShadowOffset(_textShadowOffset);
		label.SetAlignment(_textAlignment);
		++i;
	}
}

void UIListBox::Add(const String& item)
{
	size_t index = _labels.GetSize() + 1;

	UILabel& newLabel = *_labels.Emplace(this);
	newLabel.SetBounds(0.f, UICoord(1.f, index * -_itemHeight), 1.f, UICoord(0.f, _itemHeight));
	newLabel.SetFont(_font);
	newLabel.SetColour(_textColour);
	newLabel.SetShadowColour(_textShadowColour);
	newLabel.SetShadowOffset(_textShadowOffset);
	newLabel.SetString(item);
}

bool UIListBox::OnMouseDown()
{
	return _hover;
}

bool UIListBox::OnMouseUp()
{
	if (_hover && _selectedLabel)
	{
		onSelectionSubmitted(*_selectedLabel);
	}

	return _hover;
}

bool UIListBox::OnMouseMove(float x, float y, bool blocked)
{
	UIElement::OnMouseMove(x, y, blocked);

	if (_hover)
	{
		for (UILabel& label : _labels)
		{
			if (label.OverlapsPoint(x, y))
			{
				if (&label != _selectedLabel)
				{
					_selectedLabel = &label;
					_selectionBox.SetBounds(_selectedLabel->GetBounds());
					onSelectionChanged(*_selectedLabel);
				}

				break;
			}
		}	
	}
	else if (_selectedLabel)
	{
		_selectedLabel = nullptr;
		_selectionBox.SetBounds(0.f, 0.f, 0.f, 0.f);
	}

	return _hover;
}
