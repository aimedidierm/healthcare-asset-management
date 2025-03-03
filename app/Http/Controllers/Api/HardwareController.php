<?php

namespace App\Http\Controllers\Api;

use App\Enums\StockMovementType;
use App\Http\Controllers\Controller;
use App\Http\Requests\ItemStockRequest;
use App\Models\Item;
use App\Models\StockMovement;
use Symfony\Component\HttpFoundation\Response;

class HardwareController extends Controller
{
    public function addStock(ItemStockRequest $request, int $id)
    {
        $item = Item::findOrFail($id);

        StockMovement::create([
            'item_id' => $item->id,
            'quantity' => $request->input('quantity'),
            'type' => StockMovementType::Addition->value,
        ]);

        return response()->json([
            'message' => 'Stock added successfully',
            'stock' => $item->current_stock,
        ], Response::HTTP_CREATED);
    }

    public function deductStock(ItemStockRequest $request, int $id)
    {
        $item = Item::findOrFail($id);

        if ($item->current_stock < $request->input('quantity')) {
            return response()->json([
                'error' => 'Insufficient stock',
            ], Response::HTTP_BAD_REQUEST);
        }

        StockMovement::create([
            'item_id' => $item->id,
            'quantity' => -$request->input('quantity'),
            'type' => StockMovementType::Deduction->value,
        ]);

        return response()->json([
            'message' => 'Item deducted successfully',
            'stock' => $item->current_stock,
        ], Response::HTTP_CREATED);
    }
}
