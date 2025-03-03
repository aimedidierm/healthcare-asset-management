<?php

namespace App\Enums;

enum StockMovementType: string
{
    case Addition = 'addition';
    case Deduction = 'deduction';
}
